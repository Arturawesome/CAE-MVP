#include "s21_matrix_oop.h"

void S21Matrix::SubMatrix(const S21Matrix& other) {
  if ((other.row_ != row_) || (other.col_ != col_) || (matrix_ == NULL) ||
      (other.matrix_ == NULL)) {
    throw std::invalid_argument("Inavlid input format");
  }
  for (int i = 0; i < row_; ++i) {
    for (int j = 0; j < col_; ++j) {
      matrix_[i][j] -= other.matrix_[i][j];
    }
  }
}

void S21Matrix::SumMatrix(const S21Matrix& other) {
  if ((other.row_ != row_) || (other.col_ != col_) || (matrix_ == NULL) ||
      (other.matrix_ == NULL)) {
    throw std::invalid_argument("Inavlid input format");
  }
  for (int i = 0; i < row_; ++i) {
    for (int j = 0; j < col_; ++j) {
      matrix_[i][j] += other.matrix_[i][j];
    }
  }
}
