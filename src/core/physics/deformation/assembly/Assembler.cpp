
#include "Assembler.h"

#include <array>
#include <stdexcept>
#include <vector>

namespace deformation {

namespace {


std::array<int, 6> ElementDofs(const mesh::Element& element) {
    std::array<int, 6> dofs{};

    for (int i = 0; i < 3; ++i) {
        const int nodeId = element.nodes_id[i];

        dofs[2 * i]     = 2 * nodeId;
        dofs[2 * i + 1] = 2 * nodeId + 1;
    }

    return dofs;
}

fem::TriangleP1 MakeTriangle(
    const mesh::Mesh& mesh,
    int elementId
) {
    const auto& element = mesh.ElementById(elementId);

    const auto& p0 =
        mesh.NodeById(element.nodes_id[0]).position;
    const auto& p1 =
        mesh.NodeById(element.nodes_id[1]).position;
    const auto& p2 =
        mesh.NodeById(element.nodes_id[2]).position;

    return fem::TriangleP1(p0, p1, p2, elementId);
}

} // namespace

Assembler::Assembler(
    const Problem& problem,
    const mesh::Mesh& mesh
)
    : problem_(problem), mesh_(mesh) {}

LocalMatrices Assembler::MakeLocalMatrices(
    const fem::TriangleP1& p1
) const {
    LocalMatrices local;
    local.mass.setZero();
    local.stiffness.setZero();

    const double rho = problem_.Density();
    const double E = problem_.ModulYoung();
    const double nu = problem_.CoefPoisson();
    const double area = p1.Area();

    // Plane stress constitutive matrix
    Eigen::Matrix3d D = Eigen::Matrix3d::Zero();

    const double factor = E / (1.0 - nu * nu);

    D(0, 0) = factor;
    D(1, 1) = factor;
    D(0, 1) = factor * nu;
    D(1, 0) = factor * nu;
    D(2, 2) = factor * (1.0 - nu) / 2.0;

    // Strain-displacement matrix
    Eigen::Matrix<double, 3, 6> B = Eigen::Matrix<double, 3, 6>::Zero();

    const auto& grad = p1.Grad();

    for (int i = 0; i < 3; ++i) {
        const double dNdx = grad(0, i);
        const double dNdy = grad(1, i);

        B(0, 2 * i)     = dNdx;
        B(1, 2 * i + 1) = dNdy;

        B(2, 2 * i)     = dNdy;
        B(2, 2 * i + 1) = dNdx;
    }

    // Stiffness: Ke = A * B^T * D * B
    local.stiffness.noalias() =
        area * B.transpose() * D * B;

    // Consistent mass matrix
    Eigen::Matrix<double, 6, 6> M = Eigen::Matrix<double, 6, 6>::Zero();

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            const double value = (i == j) ? 2.0 : 1.0;

            M(2 * i, 2 * j) = value;
            M(2 * i + 1, 2 * j + 1) = value;
        }
    }

    local.mass = (rho * area / 12.0) * M;

    return local;
}

Eigen::Matrix<double, 6, 1> Assembler::MakeLocalForce(
    const fem::TriangleP1& p1,
    double time
) const {
    Eigen::Matrix<double, 6, 1> localForce = Eigen::Matrix<double, 6, 1>::Zero();

    const auto center = p1.Center();
    const Eigen::Vector2d bodyForce =
        problem_.Force(center.x, center.y, time);

    // Centroid quadrature for body forces
    const double weight = p1.Area() / 3.0;

    for (int i = 0; i < 3; ++i) {
        localForce(2 * i) =
            weight * bodyForce.x();

        localForce(2 * i + 1) =
            weight * bodyForce.y();
    }

    return localForce;
}

GlobalSystem Assembler::AssembleGlobalSystem() const {
    const int numDofs = 2 * mesh_.NumNodes();

    GlobalSystem global(numDofs);

    std::vector<Eigen::Triplet<double>> massTriplets;
    std::vector<Eigen::Triplet<double>> stiffnessTriplets;

    massTriplets.reserve(36 * mesh_.NumEl());
    stiffnessTriplets.reserve(36 * mesh_.NumEl());

    for (int elId = 0; elId < mesh_.NumEl(); ++elId) {
        const auto& element = mesh_.ElementById(elId);

        const auto dofs = ElementDofs(element);
        const auto triangle = MakeTriangle(mesh_, elId);

        const LocalMatrices local =
            MakeLocalMatrices(triangle);

        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                massTriplets.emplace_back(
                    dofs[i], dofs[j], local.mass(i, j)
                );

                stiffnessTriplets.emplace_back(
                    dofs[i], dofs[j], local.stiffness(i, j)
                );
            }
        }
    }

    global.mass.setFromTriplets(
        massTriplets.begin(),
        massTriplets.end()
    );

    global.stiffness.setFromTriplets(
        stiffnessTriplets.begin(),
        stiffnessTriplets.end()
    );

    global.mass.makeCompressed();
    global.stiffness.makeCompressed();

    return global;
}

void Assembler::UpdateTimeDependentData(
    GlobalSystem& global,
    double time
) const {
    const int numDofs = 2 * mesh_.NumNodes();

    if (global.force.size() != numDofs) {
        throw std::invalid_argument(
            "Deformation Assembler: invalid force vector size"
        );
    }

    global.force.setZero();

    for (int elId = 0; elId < mesh_.NumEl(); ++elId) {
        const auto& element = mesh_.ElementById(elId);

        const auto dofs = ElementDofs(element);
        const auto triangle = MakeTriangle(mesh_, elId);

        const Eigen::Matrix<double, 6, 1> localForce =
            MakeLocalForce(triangle, time);

        for (int i = 0; i < 6; ++i) {
            global.force(dofs[i]) += localForce(i);
        }
    }
}

} // namespace deformation
