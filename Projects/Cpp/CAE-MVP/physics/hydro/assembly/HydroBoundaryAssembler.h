#pragma once
#include "fem/dofs/P2DofMap.h"
#include "fem/dofs/TaylorHoodDofMap.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/hydro/problem/HydroProblem.h"
#include <unordered_map>

class HydroBoundaryAssembler2D {
  private:
    // const HydroProblem &problem_;
    // const Mesh &mesh_;
    // const P2DofMap &p2DofMap_;
    // const TaylorHoodDofMap &dofMap_;

  public:
    HydroBoundaryAssembler2D() = default;
    void Apply(LinearSystem &system, double currTime,
               const HydroProblem &problem, const Mesh &mesh,
               const P2DofMap &p2DofMap, const TaylorHoodDofMap &dofMap) const;

  private:
    std::unordered_map<int, double>
    CollectVelocityDirichlet(double currTime, const HydroProblem &problem,
                             const Mesh &mesh, const P2DofMap &p2DofMap,
                             const TaylorHoodDofMap &dofMap) const;
    void ApplyVelocityDirichlet(LinearSystem &system, double currTime,
                                const HydroProblem &problem, const Mesh &mesh,
                                const P2DofMap &p2DofMap,
                                const TaylorHoodDofMap &dofMap) const;
};