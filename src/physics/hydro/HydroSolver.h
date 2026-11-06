#pragma once
#include "fem/dofs/P2DofMap.h"
#include "fem/dofs/TaylorHoodDofMap.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/hydro/assembly/HydroAssembler.h"
#include "physics/hydro/problem/HydroProblem.h"
#include <Eigen/Dense>
#include <string>
namespace hydro {
class HydroSolver {
  private:
    const HydroProblem &problem_;
    const mesh::Mesh &mesh_;
    double timeEnd_;
    double dt_;

    fem::P2DofMap p2DofMap_;
    fem::TaylorHoodDofMap taylorHoodDofMap_;
    Eigen::VectorXd solution_;

  private:
    fem::LinearSystem BuildTimeSystem(const HydroGlobalSystem &global,
                                      const Eigen::VectorXd &previousVelocity,
                                      double dt) const;
    Eigen::VectorXd MakeInitialVelocity() const;

  public:
    explicit HydroSolver(const HydroProblem &problem, mesh::Mesh &mesh,
                         double timeEnd, double dt);
    ~HydroSolver() = default;
    void Solve(std::string filename = "solutions/hydro");
    const Eigen::VectorXd &Solution();
};
} // namespace hydro