#include "Assembler2D.h"

HeatAssembler2D::HeatAssembler2D(const HeatProblem &problem, const mesh::Mesh &mesh)
    : problem_(problem), mesh_(mesh) {}

fem::LinearSystem HeatAssembler2D::Assemble() {
    fem::LinearSystem system = fem::LinearSystem(mesh_.NumNodes());
    int numEl = mesh_.NumEl();
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(static_cast<std::size_t>(mesh_.NumEl()) * 9);
    for (int elId = 0; elId < numEl; ++elId) {
        std::array<geometry::Point2D, 3> vertices;
        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
        }
        fem::TriangleP1 triangle(vertices[0], vertices[1], vertices[2], elId);
    
        auto locF = MakeLocalF(triangle);
        auto locK = MakeLocalK(triangle);

        // collect triplets
        for (std::size_t i = 0; i < 3; ++i) {
            int node_i_id = mesh_.NodeIdFromEl(elId, i);

            for (std::size_t j = 0; j < 3; ++j) {
                int node_j_id = mesh_.NodeIdFromEl(elId, j);
                triplets.emplace_back(node_i_id, node_j_id, locK(i, j));
            }
            // collect forsec
            system.F(node_i_id) += locF(i);
        }
    }
    system.K.setFromTriplets(triplets.begin(), triplets.end());
    system.K.makeCompressed();
    return system;
}

Eigen::Vector3d HeatAssembler2D::MakeLocalF(const fem::TriangleP1 &triangle) const {
    Eigen::Vector3d locF_;

    double source = problem_.Source(triangle.Center()) / 3 * triangle.Area();
    for (std::size_t i = 0; i < 3; ++i) {
        locF_(i) = source;
    }
    return locF_;
}

Eigen::Matrix<double, 3, 3>
HeatAssembler2D::MakeLocalK(const fem::TriangleP1 &triangle) const {
    Eigen::Matrix<double, 3, 3> locK_;

    double lambda = problem_.Lambda();
    locK_ = lambda * triangle.Area() * triangle.Grad().transpose() *
            triangle.Grad();
    return locK_;
}
