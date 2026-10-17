#pragma once
#include "mesh/Mesh2D.h"
#include <Eigen/Dense>

class TriangleP2 {
  private:
    double area_ = 0;
    Point2D center_;
    void CreateGradMatrix(const Point2D &p1, const Point2D &p2,
                          const Point2D &p3);

  public:
    TriangleP2(const Mesh &mesh, int elementID);
    double Area() const;
    Point2D Center() const;
    Eigen::Matrix<double, 6, 1> ShapeFunctions(const Point2D &point) const;
    const Eigen::Matrix<double, 2, 6> Grad(const Point2D &point) const;
};