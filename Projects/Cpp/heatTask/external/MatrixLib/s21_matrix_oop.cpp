#include "s21_matrix_oop.h"

#include <iostream>

S21Matrix::S21Matrix() : row_(0), col_(0), matrix_(NULL) {}
S21Matrix::S21Matrix(int row, int col) : row_(row), col_(col) {
  if (row <= 0 || col <= 0) {
    throw std::length_error("Incorrect size of matrix");
  }
  matrix_ = new double*[row_];
  for (int i = 0; i < row_; i++) {
    matrix_[i] = new double[col_];
  }
  for (int i = 0; i < row_; i++) {
    for (int j = 0; j < col_; j++) {
      matrix_[i][j] = 0;
    }
  }
}
S21Matrix::S21Matrix(const S21Matrix& other)
    : row_(other.row_), col_(other.col_) {
  matrix_ = new double*[row_];
  for (int i = 0; i < row_; ++i) {
    matrix_[i] = new double[col_];
    for (int j = 0; j < col_; ++j) {
      matrix_[i][j] = other.matrix_[i][j];
    }
  }
}

S21Matrix::~S21Matrix() {
  if (matrix_) {
    for (int i = 0; i < row_; ++i) {
      delete[] matrix_[i];
    }
    delete[] matrix_;
  }

  row_ = 0;
  col_ = 0;
}

S21Matrix::S21Matrix(S21Matrix&& other) noexcept
    : row_(other.row_), col_(other.col_), matrix_(other.matrix_) {
  other.row_ = 0;
  other.col_ = 0;
  other.matrix_ = nullptr;
}
