#include "s21_matrix_oop.h"

S21Matrix S21Matrix::CalcComplements() const {
  S21Matrix result(row_, col_);
  if (row_ != col_) {
    throw std::invalid_argument("Matrix must be square");
  } else if (row_ == 1) {
    result(0, 0) = 1;
  } else {
    S21Matrix minor(row_ - 1, col_ - 1);
    int minus = 1;
    for (int i = 0; i < row_; i++) {
      for (int j = 0; j < col_; j++) {
        create_minor(minor, i, j);
        double det = 0;
        det = minor.Determinant();
        if ((i + j) % 2) {
          minus = -1;
        } else {
          minus = 1;
        }
        result(i, j) = minus * det;
      }
    }
  }
  return result;
}
void S21Matrix::create_minor(S21Matrix& minor, int i, int j) const {
  int count_row = 0;
  int count_col = 0;
  for (int ii = 0; ii < row_; ++ii) {
    if (ii == i) {
      continue;
    }

    for (int jj = 0; jj < col_; ++jj) {
      if (jj == j) {
        continue;
      }
      minor(count_row, count_col++) = matrix_[ii][jj];
    }
    count_row++;
    count_col = 0;
  }
}

/*

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  int flag = OK;
  if (A == NULL || result == NULL || A->matrix == NULL) {
    flag = ERR_MAT;
  } else if (A->rows != A->columns) {
    flag = ERR_CAL;
  } else if (A->rows == 1) {
    s21_create_matrix(1, 1, result);
    result->matrix[0][0] = 1;
  } else {
    int row_minor = A->rows - 1;
    int col_minor = A->columns - 1;
    int minus = 1;
    matrix_t minor = {0};
    s21_create_matrix(row_minor, col_minor, &minor);
    s21_create_matrix(row_minor + 1, col_minor + 1, result);

    for (int i = 0; i < A->rows; ++i) {
      for (int j = 0; j < A->columns; ++j) {
        create_minor(A, &minor, i, j);
        double det = 0;
        s21_determinant(&minor, &det);
        if ((i + j) % 2) {
          minus = -1;
        } else {
          minus = 1;
        }
        result->matrix[i][j] = minus * det;
      }
    }
    s21_remove_matrix(&minor);
  }

  return flag;
}

static void create_minor(matrix_t *A, matrix_t *minor, int i, int j) {
  int count_row = 0;
  int count_col = 0;
  for (int ii = 0; ii < A->rows; ++ii) {
    if (ii == i) {
      continue;
    }

    for (int jj = 0; jj < A->columns; ++jj) {
      if (jj == j) {
        continue;
      }
      minor->matrix[count_row][count_col++] = A->matrix[ii][jj];
    }
    count_row++;
    count_col = 0;
  }
}
*/