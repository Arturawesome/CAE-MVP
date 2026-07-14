#include "s21_matrix_oop.h"

int S21Matrix::GetCols() const { return col_; }
int S21Matrix::GetRows() const { return row_; }
void S21Matrix::SetColsRows(int const row, int const col) {
  if (row <= 0 || col <= 0) {
    throw std::invalid_argument("Row and col must be positive");
  }

  double** new_matrix = new double*[row];
  for (int i = 0; i < row; i++) {
    new_matrix[i] = new double[col]{};
  }

  for (int i = 0; i < row_ && i < row; ++i) {
    for (int j = 0; j < col_ && j < col; ++j) {
      new_matrix[i][j] = matrix_[i][j];
    }
  }

  for (int i = 0; i < row_; i++) {
    delete[] matrix_[i];
  }
  delete[] matrix_;
  row_ = row;
  col_ = col;
  matrix_ = new_matrix;
}
