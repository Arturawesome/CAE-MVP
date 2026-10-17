#pragma once
#include "mesh/BoundaryId.h"
enum class HeatBoundaryType { Dirichlet, Neumann, Robin };


struct HeatBoundaryCondition {
    BoundaryId boundaryId = 0;
    HeatBoundaryType type;

    double value1 = 0;
    double value2 = 0;
};