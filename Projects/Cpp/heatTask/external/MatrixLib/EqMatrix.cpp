#include "s21_matrix_oop.h"

bool S21Matrix::EqMatrix(const S21Matrix& other) {
  if ((other.GetRows() != row_) || (other.GetCols() != col_) ||
      (matrix_ == NULL) || (other.matrix_ == NULL)) {
    return false;
  }
  double epsilon = 1e-7;
  bool flag = true;
  for (int i = 0; i < row_ && flag; ++i) {
    for (int j = 0; j < col_ && flag; ++j) {
      if (fabs(other.matrix_[i][j] - matrix_[i][j]) > epsilon) {
        flag = false;
      }
    }
  }
  return flag;
}
