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
enum class EigenMethod {
  SPARSE_LU,
  SIMPLICAL_LDLT
};

class HeatSolver {
  private:
    const HeatProblem &problem_;
    const mesh::Mesh &mesh_;
     unsigned int threadCount_ = 1;
    Eigen::VectorXd solution_;
    EigenMethod method_;

  public:
    explicit HeatSolver(HeatProblem &problem,  unsigned int threadCount = 1, EigenMethod method = EigenMethod::SPARSE_LU);
    ~HeatSolver() = default;
    void Solve();
    const Eigen::VectorXd &Solution();
};
} // namespace heat