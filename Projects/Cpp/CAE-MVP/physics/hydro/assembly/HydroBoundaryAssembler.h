#pragma once
#include "fem/dofs/P2DofMap.h"
#include "fem/dofs/TaylorHoodDofMap.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/hydro/problem/HydroProblem.h"
#include <unordered_map>

class HydroBoundaryAssembler2D {
  public:
    HydroBoundaryAssembler2D() = default;
    void Apply(fem::LinearSystem &system, double currTime,
               const HydroProblem &problem, const mesh::Mesh &mesh,
               const fem::P2DofMap &p2DofMap, const fem::TaylorHoodDofMap &dofMap) const;

  private:
    std::unordered_map<int, double>
    CollectVelocityDirichlet(double currTime, const HydroProblem &problem,
                             const mesh::Mesh &mesh, const fem::P2DofMap &p2DofMap,
                             const fem::TaylorHoodDofMap &dofMap) const;
    void ApplyVelocityDirichlet(fem::LinearSystem &system, double currTime,
                                const HydroProblem &problem, const mesh::Mesh &mesh,
                                const fem::P2DofMap &p2DofMap,
                                const fem::TaylorHoodDofMap &dofMap) const;
};