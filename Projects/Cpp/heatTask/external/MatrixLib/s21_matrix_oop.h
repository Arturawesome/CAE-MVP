#ifndef S21MATRIX_OOP_H_
#define S21MATRIX_OOP_H_

#include <algorithm>
#include <cmath>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
void Clear2dArray(double** array, int row, int col);

class S21Matrix {
 private:
  int row_, col_;
  double** matrix_;

 public:
  S21Matrix();
  S21Matrix(int row, int col);
  S21Matrix(const S21Matrix& other);
  S21Matrix(S21Matrix&& other) noexcept;
  ~S21Matrix();
  double& operator()(int i, int j);
  S21Matrix operator+(const S21Matrix& other) const;
  S21Matrix operator-(const S21Matrix& other) const;
  S21Matrix operator*(const S21Matrix& other) const;
  S21Matrix operator*(const double num) const;
  friend S21Matrix operator*(const double num, S21Matrix& M);

  bool operator==(const S21Matrix& other) const;

  S21Matrix& operator+=(const S21Matrix& other);
  S21Matrix& operator-=(const S21Matrix& other);
  S21Matrix& operator*=(const double num);
  S21Matrix& operator*=(const S21Matrix& other);
  S21Matrix& operator=(const S21Matrix& other);
  void Clear2dArray(double** array, int row);

  bool EqMatrix(const S21Matrix& other);
  void SumMatrix(const S21Matrix& other);
  void SubMatrix(const S21Matrix& other);
  void MulNumber(const double num);
  void MulMatrix(const S21Matrix& other);
  double MultRowColumns(const S21Matrix& other, int row, int col);
  S21Matrix Transpose() const;
  double Determinant() const;
  S21Matrix InverseMatrix();
  S21Matrix CalcComplements() const;
  void create_minor(S21Matrix& minor, int i, int j) const;

  int GetCols() const;
  int GetRows() const;
  void SetColsRows(int const row, int const col);
};

#endif