#pragma once
#include "physics/deformation/BoundaryCondition.h"
#include <Eigen/Core>
#include <functional>
#include <vector>
namespace deformation {

class Problem {
public:
  // Internal force f(x,y,t)
  using VectorField = std::function<Eigen::Vector2d(double, double, double)>;
  using ScalarField = std::function<double(double, double, double)>;

private:
  double dencity_;
  double modulYoung_;
  double coefPoisson_;
  VectorField externalForceCallback_;
  std::vector<BoundaryCondition> boundaryConditions_;

public:
  Problem(double dencity, double modulYoung, double coefPoisson,
          VectorField externalForceCallback);
  ~Problem() = default;

  // getters
public:
  double Dencity() const;
  double ModulYoung() const;
  double CoefPoisson() const;
  const std::vector<BoundaryCondition>& BoundaryConditions() const;
  Eigen::Vector2d Force(double x, double y, double t) const;
  // boundaryCond setters
public:
  void AddDisplacementDirichlet(BoundaryId id, VectorField displacement);
  void AddDisplacementX(BoundaryId id, ScalarField displacement);
  void AddDisplacementY(BoundaryId id, ScalarField displacement);
  void AddTraction(BoundaryId id, VectorField traction);
  void AddNormalTraction(BoundaryId id, ScalarField traction);
};

} // namespace deformation
