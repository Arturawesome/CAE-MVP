#pragma once
#include <Eigen/Dense>
#include "geometry/GeometryTypes2D.h"


class TriangleP2 {
  private:
    double area_ = 0;
    Point2D center_;

    // Коэфф лин барицентрических функций.
    Eigen::Matrix<double, 3, 3> barCoefs_;

    // Постоянные градиенты L1, L2, L3 (их TriangleP1).
    Eigen::Matrix<double, 2, 3> barGrad_;

    void CreateBarycentric(const Point2D &p1, const Point2D &p2,
                          const Point2D &p3);

    void CalcArea(const Point2D &p1, const Point2D &p2, const Point2D &p3,
                  int elementID);
    
  public:
    TriangleP2(const Point2D &p1, const Point2D &p2, const Point2D &p3, int elementID);
    double Area() const;
    Point2D Center() const;
    Eigen::Matrix<double, 6, 1> Shape(const Point2D &point) const;
    Eigen::Matrix<double, 2, 6> Grad(const Point2D &point) const;
};