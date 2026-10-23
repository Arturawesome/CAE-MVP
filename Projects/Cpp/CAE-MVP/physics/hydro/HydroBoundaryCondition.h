#pragma once
#include "mesh/BoundaryId.h"
#include <functional>
#include <Eigen/Dense>

enum class HydroBoundaryConditionType {
    VelocityDirichlet,
    NormalTraction,
    Traction
};


struct HydroBoundaryCondition {
    BoundaryId boundaryId = 0;
    HydroBoundaryConditionType type;

    std::function<Eigen::Vector2d(double, double, double)> velocity;
    std::function<Eigen::Vector2d(double, double, double)> traction;
    std::function<double(double, double, double)> normalTraction; 
};