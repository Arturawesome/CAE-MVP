#pragma once

#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/deformation/assembly/Assembler.h"
#include "physics/deformation/problem/Problem.h"

#include <Eigen/Core>
#include <string>

namespace deformation {

class Solver {
public:
    Solver(const Problem& problem, const mesh::Mesh& mesh,
           double timeEnd, double dt);

    void Solve(const std::string& directory = "solutions/deformation");

    const Eigen::VectorXd& Solution() const;
    const Eigen::VectorXd& Velocity() const;
    const Eigen::VectorXd& Acceleration() const;

private:
    fem::LinearSystem BuildTimeSystem(
        const GlobalSystem& global,
        const Eigen::VectorXd& predictedDisplacement,
        double stepDt) const;

    const Problem& problem_;
    const mesh::Mesh& mesh_;
    double timeEnd_;
    double dt_;

    Eigen::VectorXd displacement_;
    Eigen::VectorXd velocity_;
    Eigen::VectorXd acceleration_;
};

} // namespace deformation
