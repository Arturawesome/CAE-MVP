#pragma once

#include "problem/Problem.h"
#include "mesh/Mesh2D.h"
#include "fem/systems/LinearSystem.h"
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <string>
#include <vector>

class HeatSolver {
  private:
    const HeatProblem &problem_;
    const Mesh &mesh_;
    Eigen::VectorXd solution_;
  
  public:
    HeatSolver(HeatProblem &problem);
    ~HeatSolver() = default;
    void Solve();
    const Eigen::VectorXd &Solution();
};