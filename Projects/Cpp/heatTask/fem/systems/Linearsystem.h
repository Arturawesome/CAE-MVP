#pragma once
#include <Eigen/Dense>

struct LinearSystem {
    Eigen::MatrixXd K;
    Eigen::VectorXd F;
    
    LinearSystem() = default;
    LinearSystem(int NumNodes) {
        if (NumNodes <= 0) {
            throw std::invalid_argument("NumNodes must be positov and mor then zero");
        }
        F = Eigen::VectorXd::Zero(NumNodes);
        K = Eigen::MatrixXd::Zero(NumNodes, NumNodes);
    }
};