#pragma once
#include "physics/deformation/problem/Problem.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include <Eigen/Dense>
#include <string>

namespace deformation {
class Solver {
private:
  const Problem &problem_;
  const mesh::Mesh &mesh_;
  double timeEnd_;
  double dt_;

  Eigen::VectorXd solution_;

private:
  fem::LinearSystem BuildTimeSystem() const;

public:
  explicit Solver(const Problem &problem, mesh::Mesh &mesh, double timeEnd,
                  double dt);
  ~Solver() = default;
  void Solve(std::string filename = "solutions/deformation");
  const Eigen::VectorXd &Solution();
};
} // namespace deformation