#include "s21_matrix_oop.h"

void S21Matrix::MulNumber(const double num) {
  if ((matrix_) && (row_ != 0 && col_ != 0)) {
    for (int i = 0; i < row_; ++i) {
      for (int j = 0; j < col_; ++j) {
        matrix_[i][j] *= num;
      }
    }
  } else {
    throw std::invalid_argument("invalid input");
  }
}
