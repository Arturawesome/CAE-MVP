#include "s21_matrix_oop.h"

S21Matrix S21Matrix::Transpose() const {
  S21Matrix trans_mat(col_, row_);

  for (int i = 0; i < row_; ++i) {
    for (int j = 0; j < col_; ++j) {
      trans_mat(j, i) = matrix_[i][j];
    }
  }

  return trans_mat;
}
