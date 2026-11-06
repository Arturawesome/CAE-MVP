#include <Eigen/Core>
#include <iostream>

#include "physics/deformation/problem/Problem.h"

int main() {
    const auto forces = [](double, double, double) -> Eigen::Vector2d {
        return Eigen::Vector2d::Zero();
    };

    deformation::Problem defTask(1000.0, 2e9, 0.3, forces);

    std::cout << "CAE-MVP: Deformation module\n";
    return 0;
}