#pragma once

#include "mesh/BoundaryId.h"
#include "physics/hydro/HydroBoundaryCondition.h"
#include <Eigen/Core>
#include <functional>
#include <vector>
class HydroProblem {
  public:
    // f(x, y, t)
    using VectorField = std::function<Eigen::Vector2d(double, double, double)>;
    using ScalarField = std::function<double(double, double, double)>;

  private:
    double viscosity_;
    double density_;
    VectorField externalForceCallback_;
    VectorField initialVelocityCallback_;

    std::vector<HydroBoundaryCondition> boundaryConditions_;

  public:
    HydroProblem(double viscosity, double density, VectorField externalForce,
                 VectorField initialVelocity);
    ~HydroProblem() = default;

    double Viscosity() const;
    double Density() const;
    Eigen::Vector2d Force(double x, double y, double t) const;
    Eigen::Vector2d InitialVelocity(double x, double y) const;
    const std::vector<HydroBoundaryCondition> &BoundaryConditions() const;

    void AddVelocityDirichlet(BoundaryId id, VectorField velocity);
    void AddTraction(BoundaryId id, VectorField traction);
    void AddOutletNormalTraction(BoundaryId id, ScalarField normalTraction);
    void AddOutletPressure(BoundaryId id, ScalarField pressure);
};