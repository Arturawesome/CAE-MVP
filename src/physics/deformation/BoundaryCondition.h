#pragma once

#include "mesh/BoundaryId.h"

#include <Eigen/Core>
#include <functional>

namespace deformation {

using VectorField =
    std::function<Eigen::Vector2d(double, double, double)>;

using ScalarField =
    std::function<double(double, double, double)>;

enum class BoundaryConditionType {
    DisplacementDirichlet,
    DisplacementX,
    DisplacementY,
    Traction,
    NormalTraction
};

struct BoundaryCondition {
    BoundaryId boundaryId = 0;
    BoundaryConditionType type;

    // Full displacement: (ux, uy)
    VectorField displacement;

    // Single displacement component: ux or uy
    ScalarField displacementComponent;

    // Surface traction: sigma * n = t
    VectorField traction;

    // Normal traction: sigma * n = q * n
    ScalarField normalTraction;
};

} // namespace deformation