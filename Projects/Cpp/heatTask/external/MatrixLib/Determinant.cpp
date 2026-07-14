#include "s21_matrix_oop.h"

double S21Matrix::Determinant() const {
  if (!matrix_ || row_ != col_) {
    throw std::invalid_argument("Invalid input");
  }
  double** A = new double*[row_];
  for (int i = 0; i < row_; ++i) {
    A[i] = new double[col_];
    for (int j = 0; j < col_; ++j) {
      A[i][j] = matrix_[i][j];
    }
  }
  double det = 1.0;
  int sign = 1;
  for (int i = 0; i < row_; ++i) {
    int max_row = i;
    double max_val = fabs(A[i][i]);
    for (int k = i + 1; k < row_; ++k) {
      if (fabs(A[k][i]) > max_val) {
        max_val = fabs(A[k][i]);
        max_row = k;
      }
    }
    if (fabs(max_val) < 1e-13) {
      det = 0.0;
      break;
    }
    if (max_row != i) {
      std::swap(A[i], A[max_row]);
      sign = -sign;
    }
    for (int k = i + 1; k < row_; ++k) {
      double factor = A[k][i] / A[i][i];
      for (int j = i; j < col_; ++j) A[k][j] -= factor * A[i][j];
    }
  }
  if (det != 0.0) {
    for (int i = 0; i < row_; ++i) det *= A[i][i];
    det *= sign;
  }
  for (int i = 0; i < row_; ++i) {
    delete[] A[i];
  }
  delete[] A;
  return det;
}

void S21Matrix::Clear2dArray(double** array, int row) {
  if (array) {
    for (int i = 0; i < row; ++i) {
      delete[] array[i];
    }
    delete[] array;
  }
}
