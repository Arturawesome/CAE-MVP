#pragma once

#include "../BoundaryCondition/BoundaryConditions.h"
#include <vector>

class HeatProblem;

// In the current implementation, boundary conditions are applied
// to boundary edges selected by their markers.
// More specific coordinate-based selection may be added later.

class SetterBoundaryCondition {
  private:
    BoundaryType type;
    std::vector<int> markers;
    std::vector<double> values;

  public:
    SetterBoundaryCondition(int type_, const std::vector<int> &markers_,
                            const std::vector<double> &values_);
    void Apply(HeatProblem &problem) const;

  private:
    void ApplyDirichlet(HeatProblem &problem) const;
    void ApplyNeumann(HeatProblem &problem) const;
};