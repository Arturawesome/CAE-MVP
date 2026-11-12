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
     unsigned int threadCount_ = 1;
    Eigen::VectorXd solution_;

  public:
    explicit HeatSolver(HeatProblem &problem,  unsigned int threadCount = 1);
    ~HeatSolver() = default;
    void Solve();
    const Eigen::VectorXd &Solution();
};
} // namespace heat