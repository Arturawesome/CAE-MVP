
#pragma once

#include "fem/elements/TriangleP1.h"
#include "mesh/Mesh2D.h"
#include "physics/deformation/problem/Problem.h"

#include <Eigen/Core>
#include <Eigen/Sparse>

namespace deformation {

struct GlobalSystem {
    Eigen::SparseMatrix<double> mass;
    Eigen::SparseMatrix<double> stiffness;
    Eigen::VectorXd force;

    explicit GlobalSystem(int numDisplacementDofs)
        : mass(numDisplacementDofs, numDisplacementDofs),
          stiffness(numDisplacementDofs, numDisplacementDofs),
          force(Eigen::VectorXd::Zero(numDisplacementDofs)) {}
};

struct LocalMatrices {
    Eigen::Matrix<double, 6, 6> mass;
    Eigen::Matrix<double, 6, 6> stiffness;
};

class Assembler {
public:
    Assembler(const Problem& problem, const mesh::Mesh& mesh);

    GlobalSystem AssembleGlobalSystem() const;

    void UpdateTimeDependentData(
        GlobalSystem& global,
        double time
    ) const;

private:
    LocalMatrices MakeLocalMatrices(
        const fem::TriangleP1& p1
    ) const;

    Eigen::Matrix<double, 6, 1> MakeLocalForce(
        const fem::TriangleP1& p1,
        double time
    ) const;

    const Problem& problem_;
    const mesh::Mesh& mesh_;
};

} // namespace deformation
