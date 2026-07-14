#include "s21_matrix_oop.h"

S21Matrix& S21Matrix::operator*=(const S21Matrix& other) {
  S21Matrix result = *this;
  this->MulMatrix(other);
  return *this;
}
S21Matrix& S21Matrix::operator*=(const double num) {
  this->MulNumber(num);
  return *this;
}

S21Matrix& S21Matrix::operator=(const S21Matrix& other) {
  if (this != &other) {
    this->SetColsRows(other.GetRows(), other.GetCols());
    for (int i = 0; i < row_; ++i) {
      for (int j = 0; j < col_; ++j) {
        matrix_[i][j] = other.matrix_[i][j];
      }
    }
  }
  return *this;
}

S21Matrix& S21Matrix::operator-=(const S21Matrix& other) {
  this->SubMatrix(other);
  return *this;
}

S21Matrix& S21Matrix::operator+=(const S21Matrix& other) {
  this->SumMatrix(other);
  return *this;
}

S21Matrix S21Matrix::operator*(const S21Matrix& other) const {
  S21Matrix result = *this;
  result *= other;
  return result;
}

bool S21Matrix::operator==(const S21Matrix& other) const {
  S21Matrix thisMatrix = *this;
  return thisMatrix.EqMatrix(other);
}
S21Matrix S21Matrix::operator*(const double num) const {
  S21Matrix result = *this;
  result *= num;
  return result;
}
S21Matrix operator*(const double num, S21Matrix& M) { return M * num; }

S21Matrix S21Matrix::operator+(const S21Matrix& other) const {
  S21Matrix result = *this;
  result += other;
  return result;
}
S21Matrix S21Matrix::operator-(const S21Matrix& other) const {
  S21Matrix result = *this;
  result -= other;
  return result;
}

double& S21Matrix::operator()(int i, int j) {
  if (i < 0 || i >= row_ || j < 0 || j >= col_) {
    throw std::out_of_range("Index out of matrix bounds");
  }
  return matrix_[i][j];
}
