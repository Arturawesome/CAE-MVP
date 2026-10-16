#pragma once

#include "../HeatProblem/HeatProblem.h"
#include "../Mesh/Mesh2D.h"
#include "systems/LinearSystem.h"
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