#pragma once


#include <Eigen/Core>
#include <functional>

namespace deformation {
class Problem {

public:
  // Internal force f(x,y,t)
  using VectorField = std::function<Eigen::Vector2d>(double, double, double);

private:
  double dencity_;
  double modulYoung_;
  double coefPoisson_;
  VectorField externalForceCallback_;

public:
  explicit Problem(double dencity, double modulYoung, double coefPoisson,
                   VectorField externalForceCallback);
  ~Problem() = default;
};

} // namespace deformation
