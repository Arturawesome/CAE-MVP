#include "HydroProblem.h"
#include <iostream>

HydroProblem::HydroProblem(double viscosity, double density,
                           VectorField externalForce,
                           VectorField initialVelocity)
    : viscosity_(viscosity), density_(density),
      externalForceCallback_(externalForce),
      initialVelocityCallback_(initialVelocity) {

    std::cout << "HydroProblem: constructor\n";
    if (viscosity <= 0 || density <= 0) {
        throw std::invalid_argument(
            "HydroProblem: viscosity and density must be postitive");
    }
    if (!externalForceCallback_ || !initialVelocityCallback_) {
        throw std::invalid_argument("HydroProblem: empty callback");
    }
}

void HydroProblem::AddVelocityDirichlet(BoundaryId id, VectorField velocity) {
    boundaryConditions_.push_back(HydroBoundaryCondition{
        .boundaryId = id,
        .type = HydroBoundaryConditionType::VelocityDirichlet,
        .velocity = velocity});
}

void HydroProblem::AddTraction(BoundaryId id, VectorField traction) {
    if (!traction) {
        throw std::invalid_argument("HydroProblem: empty traction callback");
    }
    boundaryConditions_.push_back(
        HydroBoundaryCondition{.boundaryId = id,
                               .type = HydroBoundaryConditionType::Traction,
                               .traction = traction});
}
void HydroProblem::AddOutletNormalTraction(BoundaryId id,
                                           ScalarField normalTraction) {
    if (!normalTraction) {
        throw std::invalid_argument("HydroProblem: empty traction callback");
    }
    boundaryConditions_.push_back(HydroBoundaryCondition{
        .boundaryId = id,
        .type = HydroBoundaryConditionType::NormalTraction,
        .normalTraction = normalTraction});
}

double HydroProblem::Viscosity() const { return viscosity_; }

double HydroProblem::Density() const { return density_; }

Eigen::Vector2d HydroProblem::Force(double x, double y, double t) const {
    return externalForceCallback_(x, y, t);
}

Eigen::Vector2d HydroProblem::InitialVelocity(double x, double y) const {
    return initialVelocityCallback_(x, y, 0);
}

const std::vector<HydroBoundaryCondition> &
HydroProblem::BoundaryConditions() const {
    return boundaryConditions_;
}
