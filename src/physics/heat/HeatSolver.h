#pragma once

#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "problem/Problem.h"
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <string>
#include <vector>
namespace heat {
class HeatSolver {
  private:
    const HeatProblem &problem_;
    const mesh::Mesh &mesh_;
    Eigen::VectorXd solution_;

  public:
    explicit HeatSolver(HeatProblem &problem);
    ~HeatSolver() = default;
    void Solve();
    const Eigen::VectorXd &Solution();
};
} // namespace heat