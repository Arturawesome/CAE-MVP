#pragma once
#include "fem/dofs/P2DofMap.h"
#include "fem/dofs/TaylorHoodDofMap.h"
#include "mesh/Mesh2D.h"
#include "physics/hydro/problem/HydroProblem.h"
#include <Eigen/Dense>

class HydroSolver {
  private:
    const HydroProblem &problem_;
    const Mesh &mesh_;
    double timeEnd_;
    double dt_;

    P2DofMap p2DofMap_;
    TaylorHoodDofMap taylorHoodDofMap_;
    Eigen::VectorXd solution_;
  public:
    explicit HydroSolver(const HydroProblem &problem, Mesh &mesh,
                         double timeEnd, double dt);
    ~HydroSolver() = default;
    void Solve();
    const Eigen::VectorXd &Solution();
};