#include "s21_matrix_oop.h"

S21Matrix S21Matrix::InverseMatrix() {
  if ((row_ != col_) && (row_ < 1)) {
    throw std::invalid_argument("row != col");
  }
  if (!matrix_) {
    throw std::invalid_argument("invalid input");
  }
  double det = this->Determinant();
  if (fabs(det) < 1e-7) {
    throw std::invalid_argument("det = 0");
  }
  S21Matrix inverse(row_, col_);
  if (row_ == 1) {
    inverse(0, 0) = 1.0 / (*this)(0, 0);
  } else {
    inverse = this->CalcComplements().Transpose();
    inverse *= (1 / det);
  }

  return inverse;
}