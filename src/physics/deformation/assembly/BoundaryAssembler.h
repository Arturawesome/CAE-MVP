#pragma once

#include "Assembler.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/deformation/problem/Problem.h"

namespace deformation {
class BoundaryAssembler2D {
public:
  using PrescribedDofs = std::map<int, double>;

  BoundaryAssembler2D(const Problem &problem, const mesh::Mesh &mesh);

  void AddTraction(GlobalSystem &global, double time) const;

  PrescribedDofs CollectDisplacementDirichlet(double time) const;

  void ApplyDirichlet(fem::LinearSystem &system, double time) const;

private:
  const Problem &problem_;
  const mesh::Mesh &mesh_;
};
} // namespace deformation