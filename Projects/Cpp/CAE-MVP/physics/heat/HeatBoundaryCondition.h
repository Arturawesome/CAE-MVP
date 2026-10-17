#pragma once
#include "../../models/BoundaryId.h"
enum class HeatBoundaryType { Dirichlet, Neumann, Robin };


struct BoundaryCondition {
    BoundaryId boundaryId = 0;
    HeatBoundaryType type;

    double value1 = 0;
    double value2 = 0;
};