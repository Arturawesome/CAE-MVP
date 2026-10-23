#pragma once 
#include "mesh/Mesh2D.h"
#include <Eigen/Dense>

class TriangleP1 {
    private:
        double area_ = 0;
        Point2D center_;
        Eigen::Matrix<double, 2, 3> grad_;
        Eigen::Vector3d shapeCoefs_;
        void CreateGradMatrix(const Point2D& p1,const Point2D& p2, const Point2D& p3);
    public:
        TriangleP1(const Mesh& mesh, int elementID);
        double Area() const;
        Point2D Center() const;
        const Eigen::Matrix<double, 2, 3>& Grad() const;
        Eigen::Vector3d Shape(const Point2D& p) const ;
};