#pragma once

#include "physics/heat/problem/Problem.h"
#include "mesh/Mesh2D.h"
#include "fem/systems/LinearSystem.h"


class HeatBoundaryAssembler2D {
  private:
    void addNeuman(LinearSystem &system, const Mesh &mesh,
                   BoundaryId boundaryId, double flux);

    void addDirichlet(LinearSystem &system, const Mesh &mesh, int marker,
                      double value);

    void addRobin(LinearSystem &system, const Mesh &mesh, BoundaryId boundaryId,
                  double alpha, double temperature);

  public:
    void Apply(LinearSystem &system, const HeatProblem &problem,
               const Mesh &mesh);
};