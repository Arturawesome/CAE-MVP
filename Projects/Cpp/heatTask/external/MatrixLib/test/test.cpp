#include "test.h"

TEST(COMPLEMENT_TEST, mult1) {
  S21Matrix m1(1, 1);
  m1(0, 0) = 5;
  S21Matrix res = m1.CalcComplements();
  S21Matrix expected(1, 1);
  expected(0, 0) = 1;
  ASSERT_TRUE(expected == res);
}

TEST(COMPLEMENT_TEST, mult2) {
  S21Matrix A(2, 2);
  A(0, 0) = 1;
  A(0, 1) = 2;
  A(1, 0) = 3;
  A(1, 1) = 4;

  S21Matrix expected(2, 2);
  expected(0, 0) = 4;
  expected(0, 1) = -3;
  expected(1, 0) = -2;
  expected(1, 1) = 1;

  S21Matrix res = A.CalcComplements();
  ASSERT_TRUE(expected == res);
}

TEST(COMPLEMENT_TEST, mult44) {
  S21Matrix A(10, 10);
  double values[10][10] = {
      {-0.295239, 2.099428, -1.914317, 0.880211, 1.255394, -3.404359, -3.467159,
       1.038185, 2.959254, 2.521724},
      {-1.562929, 4.047521, -2.129158, -0.586059, -4.208764, -3.499205, 1.62238,
       0.502929, 3.905122, 3.270478},
      {0.192182, 2.960357, 2.81475, 4.895878, 2.270897, -1.424739, -3.66967,
       3.149776, 1.681489, 1.338734},
      {-0.531331, -3.386005, -4.849452, -4.78221, 4.561343, -3.203851, -0.92936,
       -2.906423, 1.808963, -1.713083},
      {3.814655, 2.699485, -2.621588, -0.383868, -3.873701, 2.648887, 2.136215,
       -2.140487, -3.353915, 3.589251},
      {0.334764, -4.747226, -2.970732, 0.494614, 2.093368, 3.568576, 0.531698,
       -2.732882, 0.015761, -2.94364},
      {-3.014608, 4.855574, 4.797133, -1.324762, -1.94085, 4.323686, 2.640256,
       3.410472, -2.217691, 2.241875},
      {-0.544553, 2.41065, 2.71811, 3.318588, -2.622156, -4.395405, 0.146012,
       -2.416558, 2.508057, -1.963428},
      {-2.459552, 3.388387, -0.783554, 2.999793, -3.333957, 2.654139, 2.417262,
       2.402322, -4.310453, 0.237439},
      {-4.566317, -0.384537, 3.895123, 4.654062, -1.972301, 2.902757, -0.904864,
       2.574302, 2.368342, 3.990035}};

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      A(i, j) = values[i][j];
    }
  }

  S21Matrix expected(10, 10);
  double exp_vals[10][10] = {
      {2245413.2959893, -3026011.403946565, 1716396.7393846055,
       4183233.7734640017, 6631172.174243643, -4373876.265429395,
       12985796.085889049, 1406293.4160970978, -565759.4642696601,
       4556372.210607366},
      {-1485832.93331164, 127289.11336751717, 969050.3227334264,
       -1105485.536523266, -1393108.7993796389, 642708.7747573456,
       -5184761.329274449, -2204660.7203283184, -2696648.1857692543,
       -653658.9957556867},
      {-1982812.7631145744, -276419.0127553792, -376942.230549776,
       -3986960.2878661593, -6669049.559164001, 2050466.6268357248,
       -8187244.890446952, -711933.7302660824, -532689.2291689997,
       -3146851.637179654},
      {1862000.3837410554, 264326.640482767, -389370.0619549784,
       -2081245.8692652665, -5786829.346638352, 2927603.0276583913,
       -5884751.14523851, 2599606.791599665, 1963392.8478934132,
       -4203353.7750843605},
      {-510438.4473252872, -420974.2136509477, -251421.415783351,
       -1205014.5245538214, -1927220.2611971518, 386961.7754865406,
       -1890085.8024989131, 2547862.032803576, 1094952.7287336793,
       -2930287.1242803913},
      {-188675.2795857608, -2070739.7466699048, 1523530.805123568,
       -30920.07925038098, 390888.262602724, -3819513.806759492,
       1395981.5073254267, 739139.4452402566, -3123788.804395103,
       2799076.2317782072},
      {1097031.9957614583, -2789646.8791072313, -512141.0573027532,
       1641506.0287595757, -347794.2134944609, -2402088.1036827257,
       1554049.9757170938, 1497487.1599504515, -1024479.1384095129,
       1141873.3997786115},
      {1415576.7496490825, -1500389.8411878855, -839663.1092580117,
       -243717.1583602027, 467134.9471708918, -50081.17695361128,
       1478785.672331613, 3275600.7440924565, 307392.69574036886,
       886280.4896313798},
      {1717020.0027411373, -141182.71104640584, 1297647.8098098054,
       -1266142.5820957704, -246262.77424956596, 997538.0170126983,
       -524844.1089644893, -339139.046813924, 2102060.0961189684,
       166113.25381177352},
      {1561459.4615420436, 1702856.8833943307, -772265.8332403089,
       -1199293.0609317382, -834030.021214713, 906197.7959829625,
       -1121065.0042317517, 1317291.385010684, 901851.3801222768,
       -2905949.8709054356}};

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      expected(i, j) = exp_vals[i][j];
    }
  }

  S21Matrix res = A.CalcComplements();

  ASSERT_TRUE(expected == res);
}

TEST(COMPLEMENT_TEST, not_square) {
  S21Matrix A(2, 3);
  A(0, 0) = 1.0;
  A(0, 1) = 2.0;
  A(0, 2) = 3.0;
  A(1, 0) = 4.0;
  A(1, 1) = 5.0;
  A(1, 2) = 6.0;
  ASSERT_THROW(A.CalcComplements(), std::invalid_argument);
}

TEST(INVERSE_TEST, test1) {
  S21Matrix A(1, 1);
  A(0, 0) = 5;
  S21Matrix R = A.InverseMatrix();
  ASSERT_DOUBLE_EQ(R(0, 0), 0.2);
}

TEST(INVERSE_TEST, test2) {
  S21Matrix A(2, 2);
  A(0, 0) = 4;
  A(0, 1) = 7;
  A(1, 0) = 2;
  A(1, 1) = 6;

  S21Matrix R = A.InverseMatrix();

  ASSERT_DOUBLE_EQ(R(0, 0), 0.6);
  ASSERT_DOUBLE_EQ(R(0, 1), -0.7);
  ASSERT_DOUBLE_EQ(R(1, 0), -0.2);
  ASSERT_DOUBLE_EQ(R(1, 1), 0.4);
}

TEST(INVERSE_TEST, test3) {
  S21Matrix A(2, 2);
  A(0, 0) = 1;
  A(0, 1) = 2;
  A(1, 0) = 2;
  A(1, 1) = 4;

  ASSERT_THROW(A.InverseMatrix(), std::invalid_argument);
}

TEST(INVERSE_TEST, test4) {
  S21Matrix A(3, 2);
  A(0, 0) = 1;
  A(0, 1) = 2;
  A(1, 0) = 2;
  A(1, 1) = 4;
  A(2, 0) = 2;
  A(2, 1) = 4;

  ASSERT_THROW(A.InverseMatrix(), std::invalid_argument);
}

TEST(INVERSE_TEST, test5) {
  S21Matrix A;

  ASSERT_THROW(A.InverseMatrix(), std::invalid_argument);
}

#include "test.h"

TEST(Constructors, DefaultConstructorTest) {
  S21Matrix result;
  ASSERT_EQ(result.GetRows(), 0);
  ASSERT_EQ(result.GetCols(), 0);
}
TEST(Constructors, ConstructorTest) {
  S21Matrix result(2, 52);
  ASSERT_EQ(result.GetRows(), 2);
  ASSERT_EQ(result.GetCols(), 52);
  EXPECT_TRUE((result(0, 0) - 0.0) < EPS);
}
TEST(Constructors, CopyConstructor) {
  S21Matrix m1(2, 3);
  m1(0, 0) = 1;
  m1(0, 1) = 2;
  m1(0, 2) = 3;
  m1(1, 0) = 4;
  m1(1, 1) = 5;
  m1(1, 2) = 6321;
  S21Matrix m2 = m1;
  ASSERT_EQ(m2.GetRows(), 2);
  ASSERT_EQ(m2.GetCols(), 3);
  EXPECT_TRUE(m2 == m1);
}

TEST(Constructors, NegativeValue) {
  ASSERT_THROW(S21Matrix(1, -2), std::length_error);
}

TEST(Constructors, MoveConstructorTest) {
  S21Matrix matrix1(6, 6);
  S21Matrix matrix2(std::move(matrix1));
  ASSERT_EQ(matrix2.GetRows(), 6);
  ASSERT_EQ(matrix2.GetCols(), 6);
}

TEST(MATRIX_DET, null_matrix) {
  S21Matrix A;
  ASSERT_THROW(A.Determinant(), std::invalid_argument);
}
TEST(MATRIX_DET, matrix1) {
  S21Matrix A(1, 1);
  A(0, 0) = 1;
  ASSERT_TRUE(A.Determinant() - 1 < EPS);
}

TEST(MATRIX_DET, matrix2) {
  S21Matrix A(2, 1);
  A(0, 0) = 1;
  ASSERT_THROW(A.Determinant(), std::invalid_argument);
}

TEST(MATRIX_DET, matrix3) {
  S21Matrix A(3, 3);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      A(i, j) = (i == j ? 1.0 : 0.0);
    }
  }

  ASSERT_TRUE(A.Determinant() - 1 < EPS);
}

TEST(MATRIX_DET, matrix4) {
  S21Matrix A(3, 3);
  double vals[3][3] = {{2, 2, 2}, {4, 4, 4}, {1, 2, 3}};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      A(i, j) = vals[i][j];
    }
  }

  ASSERT_TRUE(A.Determinant() < EPS);
}

TEST(MATRIX_DET, matrix5) {
  S21Matrix A(4, 4);
  double vals[4][4] = {{1, 2, 3, 4}, {0, 5, 6, 7}, {0, 0, 8, 9}, {0, 0, 0, 10}};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A(i, j) = vals[i][j];
    }
  }

  ASSERT_TRUE(A.Determinant() - 1 * 5 * 8 * 10 < EPS);
}

TEST(MATRIX_DET, matrix6) {
  S21Matrix A(5, 5);
  double vals[5][5] = {{0.5, 0.25, 6.0, -4.0, 5.0},
                       {1.5, 0.75, 3.0, 2.0, 3.0},
                       {2.0, 1.0, 2.3, -1.0, -2.0},
                       {3.0, 4.0, 1.0, -3.0, 1.0},
                       {4.7, 5.0, 0.0, 3.0, 3.7}};
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      A(i, j) = vals[i][j];
    }
  }

  ASSERT_TRUE(A.Determinant() - 255.260 < EPS);
}

TEST(EQ_MATIX, true_eq) {
  S21Matrix m1(2, 1);
  m1(0, 0) = 2;
  m1(1, 0) = 2;

  S21Matrix m2(2, 1);
  m2(0, 0) = 2;
  m2(1, 0) = 2;
  ASSERT_TRUE(m1 == m2);
}

TEST(EQ_MATIX, false_eq) {
  S21Matrix m1(2, 1);
  m1(0, 0) = 2;
  m1(1, 0) = 2;

  S21Matrix m2(2, 1);
  m2(0, 0) = 2;
  m2(1, 0) = 0;
  ASSERT_FALSE(m1 == m2);
}

TEST(MULTIPLY_TEST, by_number) {
  S21Matrix m1(2, 2);
  double num = 14.48;
  double num1 = 10;
  m1(0, 0) = num;
  m1(0, 1) = num;
  m1(0, 0) = num;
  m1(1, 1) = num;
  S21Matrix res(2, 2);
  res(0, 0) = num * num1;
  res(0, 1) = num * num1;
  res(0, 0) = num * num1;
  res(1, 1) = num * num1;

  S21Matrix m2 = m1 * num1;
  ASSERT_TRUE(m2 == res);
}

TEST(MULTIPLY_TEST, by_number1) {
  S21Matrix m1(2, 2);
  double num = 14.48;
  double num1 = 10;
  m1(0, 0) = num;
  m1(0, 1) = num;
  m1(0, 0) = num;
  m1(1, 1) = num;
  S21Matrix res(2, 2);
  res(0, 0) = num * num1;
  res(0, 1) = num * num1;
  res(0, 0) = num * num1;
  res(1, 1) = num * num1;

  S21Matrix m2 = num1 * m1;
  ASSERT_TRUE(m2 == res);
}

TEST(MULTIPLY_TEST, by_number2) {
  S21Matrix m1(2, 2);
  double num = 14.48;
  double num1 = 10;
  m1(0, 0) = num;
  m1(0, 1) = num;
  m1(0, 0) = num;
  m1(1, 1) = num;
  S21Matrix res(2, 2);
  res(0, 0) = num * num1;
  res(0, 1) = num * num1;
  res(0, 0) = num * num1;
  res(1, 1) = num * num1;

  m1 *= num1;
  ASSERT_TRUE(m1 == res);
}

TEST(MULTIPLY_TEST, by_number3) {
  S21Matrix m1(3, 2);
  double num = 14.48;
  double num1 = -10;
  m1(0, 0) = num;
  m1(0, 1) = num;
  m1(0, 0) = num;
  m1(1, 1) = num;
  S21Matrix res(3, 2);
  res(0, 0) = num * num1;
  res(0, 1) = num * num1;
  res(0, 0) = num * num1;
  res(1, 1) = num * num1;

  m1 *= num1;
  ASSERT_TRUE(m1 == res);
}

TEST(MULTIPLY_TEST, by_matrix1) {
  S21Matrix m1(1, 1);
  S21Matrix m2(1, 1);
  S21Matrix m3(1, 1);
  S21Matrix res(1, 1);
  m1(0, 0) = 2;
  m2(0, 0) = 5;
  res(0, 0) = 10;
  m3 = m1 * m2;
  ASSERT_TRUE(m3 == res);
}

TEST(MULTIPLY_TEST, by_matrix2) {
  S21Matrix A(3, 3);
  S21Matrix B(3, 3);
  S21Matrix E(3, 3);
  S21Matrix R(3, 3);
  double valsA[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  double valsB[3][3] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
  double valsE[3][3] = {{30, 24, 18}, {84, 69, 54}, {138, 114, 90}};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      A(i, j) = valsA[i][j];
      B(i, j) = valsB[i][j];
      E(i, j) = valsE[i][j];
    }
  }

  R = A * B;
  ASSERT_TRUE(R == E);
}

TEST(MULTIPLY_TEST, by_matrix3) {
  S21Matrix A(2, 3);
  S21Matrix B(3, 2);
  S21Matrix E(2, 2);

  double valsA[2][3] = {{1, 2, 3}, {4, 5, 6}};
  double valsB[3][2] = {{7, 8}, {9, 10}, {11, 12}};
  double valsE[2][2] = {{58, 64}, {139, 154}};

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      A(i, j) = valsA[i][j];
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      B(i, j) = valsB[i][j];
    }
  }

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      E(i, j) = valsE[i][j];
    }
  }
  A *= B;
  ASSERT_TRUE(E == A);
}

TEST(MULTIPLY_TEST, by_matrix4) {
  S21Matrix m1(2, 3);
  S21Matrix m2(4, 2);
  ASSERT_THROW(m1 * m2, std::invalid_argument);
}

TEST(GetSetRowColMatrix, get_row) {
  S21Matrix m1(2, 4213);
  ASSERT_EQ(m1.GetRows(), 2);
}

TEST(GetSetRowColMatrix, get_col) {
  S21Matrix m1(2, 4213);
  ASSERT_EQ(m1.GetCols(), 4213);
}
TEST(GetSetRowColMatrix, set_col_row) {
  S21Matrix m1(1, 1);
  m1.SetColsRows(3, 3);
  ASSERT_EQ(m1.GetCols(), 3);
  ASSERT_EQ(m1.GetRows(), 3);
}

TEST(GetSetRowColMatrix, negativ_set_col_row) {
  S21Matrix m1(1, 1);
  ASSERT_THROW(m1.SetColsRows(-3, 3), std::invalid_argument);
}
TEST(SUM_SUB_TEST, norma_sum) {
  S21Matrix m1(2, 2);
  S21Matrix m2(2, 2);
  m1(0, 0) = 1;
  m1(0, 1) = 2;

  m2(0, 0) = 0;
  m2(0, 1) = 2;
  m2(1, 0) = 0;
  m2(1, 1) = -2;
  S21Matrix res = m1 + m2;
  ASSERT_EQ(res(0, 0), 1);
  ASSERT_EQ(res(0, 1), 4);
  ASSERT_EQ(res(1, 0), 0);
  ASSERT_EQ(res(1, 1), -2);
}

TEST(SUM_SUB_TEST, norma_sub) {
  S21Matrix m1(2, 2);
  S21Matrix m2(2, 2);
  m1(0, 0) = 1;
  m1(0, 1) = 2;

  m2(0, 0) = 0;
  m2(0, 1) = 2;
  m2(1, 0) = 0;
  m2(1, 1) = -2;
  S21Matrix res = m1 - m2;
  ASSERT_EQ(res(0, 0), 1);
  ASSERT_EQ(res(0, 1), 0);
  ASSERT_EQ(res(1, 0), 0);
  ASSERT_EQ(res(1, 1), 2);
}

TEST(SUM_SUB_TEST, not_norma_sub) {
  S21Matrix m1(2, 2);
  S21Matrix m2(3, 3);
  m1(0, 0) = 1;
  m1(0, 1) = 2;

  m2(0, 0) = 0;
  m2(0, 1) = 2;
  m2(1, 0) = 0;
  m2(1, 1) = -2;
  ASSERT_THROW(m1 - m2, std::invalid_argument);
}
TEST(SUM_SUB_TEST, not_norma_sum) {
  S21Matrix m1(2, 2);
  S21Matrix m2(3, 3);
  m1(0, 0) = 1;
  m1(0, 1) = 2;

  m2(0, 0) = 0;
  m2(0, 1) = 2;
  m2(1, 0) = 0;
  m2(1, 1) = -2;
  ASSERT_THROW(m1 + m2, std::invalid_argument);
}

TEST(S21MatrixUnitTest, TransposeTest) {
  S21Matrix m1(3, 2);
  m1(0, 0) = 1;
  m1(0, 1) = 2;
  m1(1, 0) = 3;
  m1(1, 1) = 4;
  m1(2, 0) = 5;
  m1(2, 1) = 6;

  S21Matrix result = m1.Transpose();

  ASSERT_EQ(result.GetRows(), 2);
  ASSERT_EQ(result.GetCols(), 3);
  ASSERT_EQ(result(0, 0), 1);
  ASSERT_EQ(result(0, 1), 3);
  ASSERT_EQ(result(0, 2), 5);
  ASSERT_EQ(result(1, 0), 2);
  ASSERT_EQ(result(1, 1), 4);
  ASSERT_EQ(result(1, 2), 6);
}

TEST(S21MatrixUnitTest, TransposeTest2) {
  S21Matrix m1(3, 1);
  m1(0, 0) = 1;
  m1(1, 0) = 3;
  m1(2, 0) = 5;

  S21Matrix result = m1.Transpose();

  ASSERT_EQ(result.GetRows(), 1);
  ASSERT_EQ(result.GetCols(), 3);
  ASSERT_EQ(result(0, 0), 1);
  ASSERT_EQ(result(0, 1), 3);
  ASSERT_EQ(result(0, 2), 5);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}