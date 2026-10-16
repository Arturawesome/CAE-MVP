#pragma once
#include "Geometry2D.h"
#include <iostream>
#include <numbers>

enum class CircleBoundary { Outer = 1 };

class Circle : public Geometry2D {
  public:
    Circle(double radius, Point2D center,
           BoundaryId boundaryID = static_cast<int>(CircleBoundary::Outer))
        : radius_(radius), center_(center), boundaryID_(boundaryID) {

        std::cout << "Circle: Created\n";

        if (radius <= 0 || !std::isfinite(radius) || !std::isfinite(center.x) ||
            !std::isfinite(center.y)) {
            throw std::invalid_argument(
                "Circle: incorrect radius or center position");
        }
    }

    bool Contains(const Point2D &p) const override {
        const double dx = p.x - center_.x;
        const double dy = p.y - center_.y;

        return dx * dx + dy * dy <= radius_ * radius_;
    }

    std::vector<Contour2D> Contours() const override {
        Contour2D contour;
        BoundaryCurve2D curve = {
            .curve = CircleArc2D{.center = center_,
                                 .radius = radius_,
                                 .startAngle = 0,
                                 .endAngle = 2 * std::numbers::pi},
            .boundaryId = boundaryID_};
        contour.curves = {curve};
        contour.isHole = false;
        return {contour};
    }

    double Radius() const {
      return radius_;
    }

    Point2D Center() const {
      return center_;
    }

    
  private:
    double radius_ = 1;
    Point2D center_;
    BoundaryId boundaryID_;
    bool isHole = false;
};
