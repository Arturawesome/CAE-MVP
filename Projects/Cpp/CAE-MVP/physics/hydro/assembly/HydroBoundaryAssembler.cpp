#include "HydroBoundaryAssembler.h"
#include <Eigen/Sparse>
#include <iostream>
#include <unordered_set>
void HydroBoundaryAssembler2D::Apply(LinearSystem &system, double currTime,
                                     const HydroProblem &problem,
                                     const Mesh &mesh, const P2DofMap &p2DofMap,
                                     const TaylorHoodDofMap &dofMap) const {

    const auto &boundCond = problem.BoundaryConditions();

    ApplyVelocityDirichlet(system, currTime, problem, mesh, p2DofMap, dofMap);
}

void HydroBoundaryAssembler2D::ApplyVelocityDirichlet(
    LinearSystem &system, double currTime, const HydroProblem &problem,
    const Mesh &mesh, const P2DofMap &p2DofMap,
    const TaylorHoodDofMap &dofMap) const {

    std::unordered_map<int, double> mapIdVelocity =
        CollectVelocityDirichlet(currTime, problem, mesh, p2DofMap, dofMap);
    std::vector<Eigen::Triplet<double>> triplets;

    for (const auto &[dofId, vel] : mapIdVelocity) {
        triplets.emplace_back(dofId, dofId, 1);
        system.F(dofId) = vel;
    }

    for (int k = 0; k < system.K.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(system.K, k); it;
             ++it) {
            int row = it.row();
            int col = it.col();
            double value = it.value();
            if (mapIdVelocity.contains(row)) {
                continue;
            }
            const auto fixedCol = mapIdVelocity.find(col);

            if (fixedCol != mapIdVelocity.end()) {
                // Переносим известную скорость в правую часть
                system.F(row) -= value * fixedCol->second;
            } else {
                triplets.emplace_back(row, col, value);
            }
        }
    }
    Eigen::SparseMatrix<double> newK(system.K.rows(), system.K.cols());
    newK.setFromTriplets(triplets.begin(), triplets.end());
    system.K = std::move(newK);

}

std::unordered_map<int, double>
HydroBoundaryAssembler2D::CollectVelocityDirichlet(
    double currTime, const HydroProblem &problem, const Mesh &mesh,
    const P2DofMap &p2DofMap, const TaylorHoodDofMap &dofMap) const {
    using Edge = std::pair<int, int>;
    struct PairHash {
        std::size_t operator()(const Edge &p) const {
            auto h1 = std::hash<int>{}(p.first);
            auto h2 = std::hash<int>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    std::unordered_map<int, double> mapIdVelocity;
    const auto &boundConds = problem.BoundaryConditions();

    for (const auto &bc : boundConds) {
        if (bc.type != HydroBoundaryConditionType::VelocityDirichlet) {
            continue;
        }

        const auto addVelocity = [&](int dofId, double value) {
            const auto [it, inserted] = mapIdVelocity.try_emplace(dofId, value);

            if (!inserted && std::abs(it->second - value) > 1e-5) {
                throw std::runtime_error("HydroBoundaryAssembler2D: "
                                         "inconsistent boundary conditions");
            }
        };

        const auto addNode = [&](int nodeId) {
            const auto &node = p2DofMap.P2NodeById(nodeId);
            const auto &pos = node.position;

            const Eigen::Vector2d velocity =
                bc.velocity(pos.x, pos.y, currTime);

            if (!velocity.allFinite()) {
                throw std::runtime_error(
                    "HydroBoundaryAssembler2D: non-finite velocity");
            }

            addVelocity(dofMap.U(nodeId), velocity.x());
            addVelocity(dofMap.V(nodeId), velocity.y());
        };

        for (const BoundaryEdge &be : mesh.BoundaryEdges()) {
            if (be.boundaryId != bc.boundaryId) {
                continue;
            }
            int id1 = be.nodes_id.first;
            int id2 = be.nodes_id.second;

            addNode(id1);
            addNode(id2);
            int middleId = p2DofMap.MiddleNodeId(id1, id2);
            addNode(middleId);
        }
    }
    return mapIdVelocity;
}
