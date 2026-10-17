#pragma once
#include "GeometryTypes2D.h"
#include "Geometry2D.h"
#include <iostream>

enum class RectangleBoundary { Bottom = 1, Right, Top, Left };

class Rectangle : public Geometry2D {
  private:
    double height_ = 1;
    double width_ = 1;
    Point2D origin_;

  public:
    Rectangle(double height, double width) : height_(height), width_(width) {
        if (!std::isfinite(width_) || !std::isfinite(height_) ||
            width_ <= 0.0 || height_ <= 0.0) {
            throw std::invalid_argument("Rectangle: invalid geometry");
        }

    }
    

    Rectangle(double height, double width, Point2D origin)
        : height_(height), width_(width), origin_(origin) {        
        if (!std::isfinite(width_) || !std::isfinite(height_) ||
            !std::isfinite(origin_.x) || !std::isfinite(origin_.y) ||
            width_ <= 0.0 || height_ <= 0.0) {
            throw std::invalid_argument("Rectangle: invalid geometry");
        }

        std::cout << "Rectange h, w, o: Created\n";
    }


    std::vector<Contour2D> Contours() const override {
        Point2D p1{origin_.x, origin_.y};
        Point2D p2{origin_.x + width_, origin_.y};
        Point2D p3{origin_.x + width_, origin_.y + height_};
        Point2D p4{origin_.x, origin_.y + height_};

        Contour2D countour;
        countour.curves = {
            {LineSegment2D{.a = p1, .b = p2},
             .boundaryId = static_cast<int>(RectangleBoundary::Bottom)},
            {LineSegment2D{.a = p2, .b = p3},
             .boundaryId = static_cast<int>(RectangleBoundary::Right)},
            {LineSegment2D{.a = p3, .b = p4},
             .boundaryId = static_cast<int>(RectangleBoundary::Top)},
            {LineSegment2D{.a = p4, .b = p1},
             .boundaryId = static_cast<int>(RectangleBoundary::Left)}};
        countour.isHole = false;

        std::vector<Contour2D> outer;
        outer.push_back(countour);

        return outer;
    }


    bool Contains(const Point2D &p) const override {
        std::cout << "Rectangle: Contains\n";
        bool is_Contains = p.x >= origin_.x && p.x <= origin_.x + width_ &&
                           p.y >= origin_.y && p.y <= origin_.y + height_;

        return is_Contains;
    }


    double Height() const {
        std::cout << "Rectangle: getHeight\n";
        return height_;
    }


    double Width() const {
        std::cout << "Rectangle: getWidth\n";
        return width_;
    }


    Point2D Origin() const {
        std::cout << "Rectangle: Origin\n";
        return origin_;
    }
};