#include "Solver.h"
#include <iostream>
namespace deformation {
Solver::Solver(const Problem &problem, mesh::Mesh &mesh, double timeEnd,
               double dt)
    : problem_(problem), mesh_(mesh), timeEnd_(timeEnd), dt_(dt) {

  solution_ = Eigen::VectorXd::Zero(mesh_.NumNodes());
  if (dt_ <= 0.0 || timeEnd_ <= 0.0) {
    throw std::invalid_argument("deformation::Solver: invalid time parameters");
  }
  std::cout<<"deformation::solver\n";
}


} // namespace deformation