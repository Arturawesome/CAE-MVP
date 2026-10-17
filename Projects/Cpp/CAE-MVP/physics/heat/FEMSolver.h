#pragma once

#include "problem/Problem.h"
#include "../../mesh/Mesh2D.h"
#include "../../fem/systems/LinearSystem.h"
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <string>
#include <vector>

class FEMSolver {
  private:
    const HeatProblem &problem;
    const Mesh &mesh;
    Eigen::VectorXd solution;
  
  public:
    FEMSolver(HeatProblem &problem_);
    ~FEMSolver() = default;
    void Solve();
    const Eigen::VectorXd &Solution();
};