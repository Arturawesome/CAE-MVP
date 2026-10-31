#pragma once
#include "Geometry2D.h"
#include "GeometryTypes2D.h"
#include <iostream>
namespace geometry {
enum class RectangleWithHoleBoundary { Bottom = 1, Right, Top, Left, Hole };

class RectangleWithHole : public Geometry2D {
  private:
    double height_ = 1;
    double width_ = 1;
    double holeRadius_ = height_ / 4;
    Point2D holeCenter_ = Point2D{};
    Point2D origin_;

  public:
    RectangleWithHole(double height, double width, double holeRadius,
                      Point2D holeCenter, Point2D origin)
        : height_(height), width_(width), holeRadius_(holeRadius),
          holeCenter_(holeCenter), origin_(origin) {

        if (!std::isfinite(width_) || !std::isfinite(height_) ||
            !std::isfinite(holeRadius_) || !std::isfinite(origin_.x) ||
            !std::isfinite(origin_.y) || !std::isfinite(holeCenter_.x) ||
            !std::isfinite(holeCenter_.y) || width_ <= 0.0 || height_ <= 0.0 ||
            holeRadius_ <= 0.0) {

            throw std::invalid_argument(
                "RectangleWithHole: invalid geometry parameters");
        }

        const double xmin = origin_.x;
        const double xmax = origin_.x + width_;
        const double ymin = origin_.y;
        const double ymax = origin_.y + height_;

        if (holeCenter_.x - holeRadius_ <= xmin ||
            holeCenter_.x + holeRadius_ >= xmax ||
            holeCenter_.y - holeRadius_ <= ymin ||
            holeCenter_.y + holeRadius_ >= ymax) {

            throw std::invalid_argument(
                "RectangleWithHole: hole must lie strictly inside rectangle");
        }
    }

    std::vector<Contour2D> Contours() const override {
        Point2D p1{origin_.x, origin_.y};
        Point2D p2{origin_.x + width_, origin_.y};
        Point2D p3{origin_.x + width_, origin_.y + height_};
        Point2D p4{origin_.x, origin_.y + height_};

        Contour2D countour;
        countour.curves = {
            {.curve = LineSegment2D{.a = p1, .b = p2},
             .boundaryId = static_cast<int>(RectangleWithHoleBoundary::Bottom)},
            {.curve = LineSegment2D{.a = p2, .b = p3},
             .boundaryId = static_cast<int>(RectangleWithHoleBoundary::Right)},
            {.curve = LineSegment2D{.a = p3, .b = p4},
             .boundaryId = static_cast<int>(RectangleWithHoleBoundary::Top)},
            {.curve = LineSegment2D{.a = p4, .b = p1},
             .boundaryId = static_cast<int>(RectangleWithHoleBoundary::Left)}};
        countour.isHole = false;

        std::vector<Contour2D> outer;
        outer.push_back(countour);

        Contour2D hole;
        hole.curves = {
            {.curve = CircleArc2D{.center = holeCenter_,
                                  .radius = holeRadius_,
                                  .startAngle = 2.0 * std::numbers::pi,
                                  .endAngle = 0.0},
             .boundaryId = static_cast<int>(RectangleWithHoleBoundary::Hole)}};
        hole.isHole = true;

        outer.push_back(hole);
        return outer;
    }

    bool Contains(const Point2D &p) const override {
        std::cout << "Rectangle: Contains\n";
        bool is_Contains = p.x >= origin_.x && p.x <= origin_.x + width_ &&
                           p.y >= origin_.y && p.y <= origin_.y + height_ &&
                           ((p.x - holeCenter_.x) * (p.x - holeCenter_.x) +
                                (p.y - holeCenter_.y) * (p.y - holeCenter_.y) >
                            holeRadius_ * holeRadius_);

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

    Point2D HoleCenter() const {
        std::cout << "Rectangle: Origin\n";
        return holeCenter_;
    }
    double HoleRadius() const { return holeRadius_; }
};

} // namespace geometrys