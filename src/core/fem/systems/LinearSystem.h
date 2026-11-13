#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <utility>
namespace fem {
struct LinearSystem {
  Eigen::SparseMatrix<double> K;

  Eigen::VectorXd F;

  LinearSystem() = default;
  ~LinearSystem() = default;
  LinearSystem(int NumNodes) {
    if (NumNodes <= 0) {
      throw std::invalid_argument("NumNodes must be positov and mor then zero");
    }
    F = Eigen::VectorXd::Zero(NumNodes);
    K = Eigen::SparseMatrix<double>(NumNodes, NumNodes);
  }
  LinearSystem(const LinearSystem &) = default;
  LinearSystem &operator=(const LinearSystem &) = default;

  LinearSystem(LinearSystem &&) noexcept = default;
  LinearSystem &operator=(LinearSystem &&) noexcept = default;
};

} // namespace fem