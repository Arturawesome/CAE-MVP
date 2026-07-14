#include "s21_matrix_oop.h"

void S21Matrix::MulMatrix(const S21Matrix& other) {
  if ((matrix_) && (other.matrix_) && (row_ > 0 && col_ > 0) &&
      (other.row_ > 0 && other.col_ > 0) && (col_ == other.row_)) {
    double** result = new double*[row_];
    for (int i = 0; i < row_; i++) {
      result[i] = new double[other.col_]{};
    }
    bool flag = true;
    for (int i = 0; i < row_ && flag; ++i) {
      for (int j = 0; j < other.col_ && flag; ++j) {
        double tmp = MultRowColumns(other, i, j);
        if (std::isfinite(tmp)) {
          result[i][j] = tmp;
        } else {
          Clear2dArray(result, row_);
          flag = false;
        }
      }
    }
    for (int i = 0; i < row_; i++) {
      delete[] matrix_[i];
    }
    delete[] matrix_;
    matrix_ = result;
    col_ = other.col_;

  } else {
    throw std::invalid_argument("invalid input");
  }
}

double S21Matrix::MultRowColumns(const S21Matrix& other, int row, int col) {
  double result = 0.0;
  for (int i = 0; i < col_; ++i) {
    result += matrix_[row][i] * other.matrix_[i][col];
  }
  return result;
}
