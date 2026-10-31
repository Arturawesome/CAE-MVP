#pragma once

#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/heat/problem/Problem.h"
#include <unordered_map>

namespace heat {
class HeatBoundaryAssembler2D {
  private:
    std::unordered_map<int, double>
    CollectDirichlet(const HeatProblem &problem, const mesh::Mesh &mesh) const;
    void AddNeumann(fem::LinearSystem &system, const mesh::Mesh &mesh,
                    BoundaryId boundaryId, double flux) const;
    void AddRobin(fem::LinearSystem &system, const mesh::Mesh &mesh,
                  BoundaryId boundaryId, double alpha,
                  double temperature) const;
    void
    ApplyDirichlet(fem::LinearSystem &system,
                   const std::unordered_map<int, double> &fixedNodes) const;

  public:
    void Apply(fem::LinearSystem &system, const HeatProblem &problem,
               const mesh::Mesh &mesh) const;
};
} // namespace heat