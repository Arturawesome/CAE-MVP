#pragma once

#include "../../HeatProblem/HeatProblem.h"
#include "../../Mesh/Mesh2D.h"
#include "../systems/LinearSystem.h"

class HeatBoundaryAssembler2D {
  private:
    void addNeuman(LinearSystem &system, const Mesh &mesh,
                   BoundaryId boundaryId, double flux);

    void addDirichlet(LinearSystem &system, const Mesh &mesh, int marker,
                      double value);

    void addRobin();//LinearSystem &system, const Mesh &mesh, BoundaryId boundaryId,
                  //double value1, double value2);

  public:
    void Apply(LinearSystem &system, const HeatProblem &problem,
               const Mesh &mesh);
};