#pragma once
#include "geometry/GeometryTypes2D.h"
#include <Eigen/Dense>
#include <array>
namespace fem {

struct TriangleQuadraturePoint {
    double L1;
    double L2;
    double L3;
    double weight;
};

struct EdgeQuadraturePoint {
    double pos;
    double weight;
};

std::array<TriangleQuadraturePoint, 6> Make6QuadraturePoints();
std::array<EdgeQuadraturePoint, 3> MakeEdgeQuadraturePoints();
std::array<TriangleQuadraturePoint, 7> Make7QuadraturePoints();

class TriangleP2 {
  private:
    double area_ = 0;
    geometry::Point2D center_;

    // Коэфф лин барицентрических функций.
    Eigen::Matrix<double, 3, 3> barCoefs_;

    // Постоянные градиенты L1, L2, L3 (их TriangleP1).
    Eigen::Matrix<double, 2, 3> barGrad_;

    void CreateBarycentric(const geometry::Point2D &p1,
                           const geometry::Point2D &p2,
                           const geometry::Point2D &p3);

    void CalcArea(const geometry::Point2D &p1, const geometry::Point2D &p2,
                  const geometry::Point2D &p3, int elementID);

  public:
    TriangleP2(const geometry::Point2D &p1, const geometry::Point2D &p2,
               const geometry::Point2D &p3, int elementID);
    double Area() const;
    geometry::Point2D Center() const;
    Eigen::Matrix<double, 6, 1> Shape(const geometry::Point2D &point) const;
    Eigen::Matrix<double, 2, 6> Grad(const geometry::Point2D &point) const;
};
} // namespace fem