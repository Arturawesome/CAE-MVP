#include "Problem.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace deformation {

Problem::Problem(double dencity, double modulYoung, double coefPoisson,
                 VectorField externalForceCallback)
    : dencity_(dencity), modulYoung_(modulYoung), coefPoisson_(coefPoisson),
      externalForceCallback_(std::move(externalForceCallback)) {

  std::cout << "Deformation: constructor\n";
  if (dencity_ <= 0 || modulYoung_ <= 0) {
    throw std::invalid_argument(
        "Deformation: modulYoung and density must be postitive");
  }
  if (!externalForceCallback_) {
    throw std::invalid_argument("Deformation: empty callback");
  }
}

double Problem::Dencity() const { return dencity_; }
double Problem::ModulYoung() const { return modulYoung_; }
double Problem::CoefPoisson() const { return coefPoisson_; }
Eigen::Vector2d Problem::Force(double x, double y, double t) const {
  return externalForceCallback_(x, y, t);
}
const std::vector<BoundaryCondition> &Problem::BoundaryConditions() const {
  return boundaryConditions_;
}

void Problem::AddDisplacementDirichlet(BoundaryId id,
                                       VectorField displacement) {
  if (!displacement) {
    throw std::invalid_argument("Empty displacement callback");
  }
  boundaryConditions_.push_back(
      BoundaryCondition{.boundaryId = id,
                        .type = BoundaryConditionType::DisplacementDirichlet,
                        .displacement = std::move(displacement)});
}

void Problem::AddDisplacementX(BoundaryId id, ScalarField displacement) {
  if (!displacement) {
    throw std::invalid_argument("Empty X displacement callback");
  }
  BoundaryCondition bc;
  bc.boundaryId = id;
  bc.type = BoundaryConditionType::DisplacementX;
  bc.displacementComponent = std::move(displacement);
  boundaryConditions_.push_back(std::move(bc));
}

void Problem::AddDisplacementY(BoundaryId id, ScalarField displacement) {
  if (!displacement) {
    throw std::invalid_argument("Empty Y displacement callback");
  }

  BoundaryCondition bc;
  bc.boundaryId = id;
  bc.type = BoundaryConditionType::DisplacementY;
  bc.displacementComponent = std::move(displacement);

  boundaryConditions_.push_back(std::move(bc));
}

void Problem::AddTraction(BoundaryId id, VectorField traction) {
  if (!traction) {
    throw std::invalid_argument("Empty traction callback");
  }

  BoundaryCondition bc;
  bc.boundaryId = id;
  bc.type = BoundaryConditionType::Traction;
  bc.traction = std::move(traction);

  boundaryConditions_.push_back(std::move(bc));
}

void Problem::AddNormalTraction(BoundaryId id, ScalarField normalTraction) {
  if (!normalTraction) {
    throw std::invalid_argument("Empty normal traction callback");
  }

  BoundaryCondition bc;
  bc.boundaryId = id;
  bc.type = BoundaryConditionType::NormalTraction;
  bc.normalTraction = std::move(normalTraction);

  boundaryConditions_.push_back(std::move(bc));
}

} // namespace deformation
