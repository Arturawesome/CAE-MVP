#pragma once
#include <Eigen/Dense>

struct LinearSystem {
    Eigen::MatrixXd K;
    Eigen::VectorXd F;
};