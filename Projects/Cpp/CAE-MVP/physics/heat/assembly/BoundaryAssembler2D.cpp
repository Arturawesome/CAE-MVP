#include "BoundaryAssembler2D.h"

#include <Eigen/Sparse>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>



double EdgeLength(const Mesh& mesh, const BoundaryEdge& edge) {
    const auto& a = mesh.NodeById(edge.nodes_id.first).position;
    const auto& b = mesh.NodeById(edge.nodes_id.second).position;
    const double length = std::hypot(b.x - a.x, b.y - a.y);
    if (!std::isfinite(length) || length <= 0.0) {
        throw std::runtime_error("HeatBoundaryAssembler2D: invalid boundary edge length");
    }
    return length;
}

bool SameTemperature(double a, double b) {
    constexpr double tolerance = 1e-10;
    return std::abs(a - b) <= tolerance * std::max({1.0, std::abs(a), std::abs(b)});
}



void HeatBoundaryAssembler2D::Apply(LinearSystem& system,
                                    const HeatProblem& problem,
                                    const Mesh& mesh) const {
    if (system.K.rows() != system.K.cols() ||
        system.K.rows() != mesh.NumNodes() || system.F.size() != mesh.NumNodes()) {
        throw std::invalid_argument("HeatBoundaryAssembler2D: system size does not match mesh");
    }

    // Natural boundary conditions must be assembled before eliminating fixed DOFs.
    for (const auto& cond : problem.HeatBoundaryConditions()) {
        switch (cond.type) {
        case HeatBoundaryType::Neumann:
            AddNeumann(system, mesh, cond.boundaryId, cond.value1);
            break;
        case HeatBoundaryType::Robin:
            AddRobin(system, mesh, cond.boundaryId, cond.value1, cond.value2);
            break;
        case HeatBoundaryType::Dirichlet:
            break;
        }
    }

    ApplyDirichlet(system, CollectDirichlet(problem, mesh));
}

std::unordered_map<int, double> HeatBoundaryAssembler2D::CollectDirichlet(
    const HeatProblem& problem, const Mesh& mesh) const {
    std::unordered_map<int, double> fixedNodes;

    for (const auto& cond : problem.HeatBoundaryConditions()) {
        if (cond.type != HeatBoundaryType::Dirichlet) {
            continue;
        }
        if (!std::isfinite(cond.value1)) {
            throw std::invalid_argument("Dirichlet: temperature must be finite");
        }

        std::size_t processedEdges = 0;
        for (const auto& edge : mesh.BoundaryEdges()) {
            if (edge.boundaryId != cond.boundaryId) {
                continue;
            }
            ++processedEdges;
            for (int nodeId : {edge.nodes_id.first, edge.nodes_id.second}) {
                const auto [it, inserted] = fixedNodes.try_emplace(nodeId, cond.value1);
                if (!inserted && !SameTemperature(it->second, cond.value1)) {
                    throw std::runtime_error(
                        "Dirichlet: conflicting temperatures at node " + std::to_string(nodeId));
                }
            }
        }
        if (processedEdges == 0) {
            throw std::runtime_error("Dirichlet: no boundary edges with marker " +
                                     std::to_string(cond.boundaryId));
        }
    }
    return fixedNodes;
}

void HeatBoundaryAssembler2D::AddNeumann(LinearSystem& system, const Mesh& mesh,
                                        BoundaryId boundaryId, double flux) const {
    if (!std::isfinite(flux)) {
        throw std::invalid_argument("Neumann: flux must be finite");
    }
    std::size_t processedEdges = 0;
    for (const auto& edge : mesh.BoundaryEdges()) {
        if (edge.boundaryId != boundaryId) {
            continue;
        }
        const double contribution = flux * EdgeLength(mesh, edge) / 2.0;
        system.F(edge.nodes_id.first) += contribution;
        system.F(edge.nodes_id.second) += contribution;
        ++processedEdges;
    }
    if (processedEdges == 0) {
        throw std::runtime_error("Neumann: no boundary edges with marker " +
                                 std::to_string(boundaryId));
    }
}

void HeatBoundaryAssembler2D::AddRobin(LinearSystem& system, const Mesh& mesh,
                                      BoundaryId boundaryId, double alpha,
                                      double temperature) const {
    if (!std::isfinite(alpha) || alpha < 0.0) {
        throw std::invalid_argument("Robin: alpha must be finite and non-negative");
    }
    if (!std::isfinite(temperature)) {
        throw std::invalid_argument("Robin: ambient temperature must be finite");
    }

    std::vector<Eigen::Triplet<double>> triplets;
    std::size_t processedEdges = 0;
    for (const auto& edge : mesh.BoundaryEdges()) {
        if (edge.boundaryId != boundaryId) {
            continue;
        }
        const int a = edge.nodes_id.first;
        const int b = edge.nodes_id.second;
        const double length = EdgeLength(mesh, edge);
        const double k = alpha * length / 6.0;
        const double f = alpha * temperature * length / 2.0;
        triplets.emplace_back(a, a, 2.0 * k);
        triplets.emplace_back(a, b, k);
        triplets.emplace_back(b, a, k);
        triplets.emplace_back(b, b, 2.0 * k);
        system.F(a) += f;
        system.F(b) += f;
        ++processedEdges;
    }
    if (processedEdges == 0) {
        throw std::runtime_error("Robin: no boundary edges with marker " +
                                 std::to_string(boundaryId));
    }

    Eigen::SparseMatrix<double> robinK(system.K.rows(), system.K.cols());
    robinK.setFromTriplets(triplets.begin(), triplets.end());
    system.K += robinK;
}

void HeatBoundaryAssembler2D::ApplyDirichlet(
    LinearSystem& system, const std::unordered_map<int, double>& fixedNodes) const {
    if (fixedNodes.empty()) {
        return;
    }

    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(static_cast<std::size_t>(system.K.nonZeros()) + fixedNodes.size());

    for (int outer = 0; outer < system.K.outerSize(); ++outer) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(system.K, outer); it; ++it) {
            const int row = it.row();
            const int col = it.col();
            if (fixedNodes.contains(row)) {
                continue;
            }
            if (const auto fixed = fixedNodes.find(col); fixed != fixedNodes.end()) {
                system.F(row) -= it.value() * fixed->second;
            } else {
                triplets.emplace_back(row, col, it.value());
            }
        }
    }

    for (const auto& [nodeId, temperature] : fixedNodes) {
        triplets.emplace_back(nodeId, nodeId, 1.0);
        system.F(nodeId) = temperature;
    }

    Eigen::SparseMatrix<double> constrainedK(system.K.rows(), system.K.cols());
    constrainedK.setFromTriplets(triplets.begin(), triplets.end());
    system.K.swap(constrainedK);
}
