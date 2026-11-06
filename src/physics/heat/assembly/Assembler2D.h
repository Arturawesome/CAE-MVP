#pragma once
#include <Eigen/Dense>

#include "fem/elements/TriangleP1.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/heat/problem/Problem.h"
namespace heat {
class HeatAssembler2D {
  private:
    const HeatProblem &problem_;
    const mesh::Mesh &mesh_;

  public:
    HeatAssembler2D(const HeatProblem &problem, const mesh::Mesh &mesh);
    fem::LinearSystem Assemble();

  private:
    Eigen::Vector3d MakeLocalF(const fem::TriangleP1 &triangle) const;
    Eigen::Matrix<double, 3, 3>
    MakeLocalK(const fem::TriangleP1 &triangle) const;
};
} // namespace heat