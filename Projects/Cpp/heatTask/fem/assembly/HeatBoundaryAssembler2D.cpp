#include "HeatBoundaryAssembler2D.h"
#include <unordered_set>

void HeatBoundaryAssembler2D::Apply(LinearSystem &system,
                                    const HeatProblem &problem,
                                    const Mesh &mesh) {

    const auto& boundConds = problem.BoundaryConditions();
    for (const auto& cond : boundConds) {
        switch (cond.type) {
        case HeatBoundaryType::Neumann:
            addNeuman(system, mesh, cond.boundaryId, cond.value1);
            break;
        case HeatBoundaryType::Robin:
            addRobin();//system, mesh, cond.boundaryId, cond.value1, cond.value2);
            break;
        case HeatBoundaryType::Dirichlet:
            break;
        }
    }
    for (const auto &cond : boundConds) {
        if (cond.type == HeatBoundaryType::Dirichlet) {
            addDirichlet(system, mesh, cond.boundaryId, cond.value1);
        }
    }
}

void HeatBoundaryAssembler2D::addNeuman(LinearSystem &system, const Mesh &mesh,
                                        BoundaryId boundaryId, double flux) {
    std::size_t processedEdges = 0;

    for (const BoundaryEdge &be : mesh.BoundaryEdges()) {
        if (be.boundaryId != boundaryId) {
            continue;
        }

        int id1 = be.nodes_id.first;
        int id2 = be.nodes_id.second;

        const Node &node1 = mesh.NodeById(id1);
        const Node &node2 = mesh.NodeById(id2);

        double dx = node2.position.x - node1.position.x;
        double dy = node2.position.y - node1.position.y;

        const double edgeLength = std::hypot(dx, dy);
        if (edgeLength <= 0.0) {
            throw std::runtime_error(
                "FEMSolver::applyNeumannToEdge: zero-length boundary edge");
        }

        double localContribution = flux * 0.5 * edgeLength;

        system.F[id1] += localContribution;
        system.F[id2] += localContribution;

        ++processedEdges;
    }
    if (processedEdges == 0) {
        throw std::runtime_error("FEMSolver: no boundary edges with marker " +
                                 std::to_string(boundaryId));
    }

    return;
}

void HeatBoundaryAssembler2D::addDirichlet(LinearSystem &system,
                                           const Mesh &mesh,  BoundaryId boundaryId,
                                           double value) {
    std::unordered_set<int> nodes;
    for (const BoundaryEdge &be : mesh.BoundaryEdges()) {
        if (be.boundaryId != boundaryId) {
            continue;
        }
        nodes.insert(be.nodes_id.first);
        nodes.insert(be.nodes_id.second);
    }
    if (nodes.empty()) {
        throw std::runtime_error("FEMSolver: no boundary edges with marker " +
                                 std::to_string(boundaryId));
    }
    for (const int nodeId : nodes) {
        const Eigen::Index s = static_cast<Eigen::Index>(nodeId);
        if (s < 0 || s >= system.K.rows()) {
            throw std::out_of_range("FEMSolver: invalid Dirichlet node index");
        }

        system.F -= system.K.col(s) * value;

        system.K.row(s).setZero();
        system.K.col(s).setZero();

        system.K(s, s) = 1.0;
        system.F(s) = value;
    }
}

void HeatBoundaryAssembler2D::addRobin() {
    throw std::logic_error("Robin boundary condition is not implemented");
}
