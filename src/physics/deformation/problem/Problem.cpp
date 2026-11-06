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

} // namespace deformation
