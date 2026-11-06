#pragma once
#include "mesh/BoundaryId.h"

namespace heat {
enum class HeatBoundaryType { Dirichlet, Neumann, Robin };

struct HeatBoundaryCondition {
    BoundaryId boundaryId = 0;
    HeatBoundaryType type;

    double value1 = 0;
    double value2 = 0;
};

} // namespace heat