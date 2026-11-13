#pragma once
#include "mesh/Mesh2D.h"
#include <Eigen/Dense>
namespace fem {

class TriangleP1 {
  private:
    double area_ = 0;
    geometry::Point2D center_;
    Eigen::Matrix<double, 2, 3> grad_;
    Eigen::Vector3d shapeCoefs_;
    void CreateGradMatrix(const geometry::Point2D &p1,
                          const geometry::Point2D &p2,
                          const geometry::Point2D &p3);

  public:
   
    TriangleP1(const geometry::Point2D &p1, const geometry::Point2D &p2,
               const geometry::Point2D &p3, int elementID);
    double Area() const;
    geometry::Point2D Center() const;
    const Eigen::Matrix<double, 2, 3> &Grad() const;
    Eigen::Vector3d Shape(const geometry::Point2D &p) const;
};
} // namespace fem