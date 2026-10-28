#pragma once
#include "fem/dofs/P2DofMap.h"
#include "fem/dofs/TaylorHoodDofMap.h"
#include "mesh/Mesh2D.h"
#include "physics/hydro/problem/HydroProblem.h"
#include "physics/hydro/assembly/HydroAssembler.h"
#include "fem/systems/LinearSystem.h"
#include <Eigen/Dense>
#include <string>

class HydroSolver {
  private:
    const HydroProblem &problem_;
    const Mesh &mesh_;
    double timeEnd_;
    double dt_;

    P2DofMap p2DofMap_;
    TaylorHoodDofMap taylorHoodDofMap_;
    Eigen::VectorXd solution_;

  private:
    LinearSystem BuildTimeSystem(const HydroGlobalSystem &global,
                                 const Eigen::VectorXd &previousVelocity,
                                 double dt) const;
    Eigen::VectorXd MakeInitialVelocity() const;

  public:
    explicit HydroSolver(const HydroProblem &problem, Mesh &mesh,
                         double timeEnd, double dt);
    ~HydroSolver() = default;
    void Solve(std::string filename = "solutions/hydro");
    const Eigen::VectorXd &Solution();
};