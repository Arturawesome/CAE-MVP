#pragma once

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <variant>
#include <vector>
#include "BoundaryId.h"


struct Point2D {
    double x = 0.0;
    double y = 0.0;
};


// set the boundary segment (ALL side of figure, not NODE or edje of element ALL SIDE)
struct LineSegment2D {
    Point2D a;
    Point2D b;
    Point2D PointAt(double t) const {
        return Point2D{.x = a.x + t * (b.x - a.x), .y = a.y + t * (b.y - a.y)};
    }
    double Lenght() const { return std::hypot(b.x - a.x, b.y - a.x); }
};


struct CircleArc2D {
    Point2D center;
    double radius = 1;
    double startAngle = 0;
    double endAngle = 2.0 * std::numbers::pi;

    Point2D PointAt(double t) const {
        double angle = startAngle + t * (endAngle - startAngle);
        return Point2D{center.x + radius * std::cos(angle),
                       center.y + radius * std::sin(angle)};
    }
    double Lenght() const { return radius * std::abs(endAngle - startAngle); }
};



using Curve2D = std::variant<LineSegment2D, CircleArc2D>;

struct BoundaryCurve2D {
    Curve2D curve;
    BoundaryId boundaryId;

    Point2D PointAt(double t) const {
        if (!std::isfinite(t) || t < 0.0 || t > 1.0) {
            throw std::out_of_range("Curve parameter must be in [0, 1]");
        }
        return std::visit([t](const auto &curve) { return curve.PointAt(t); },
                          curve);
    }

    double Lenght() const {
        return std::visit([](const auto &curve) { return curve.Lenght(); },
                          curve);
    }

};

// vector of boundaries in our geometry
struct Contour2D {
    std::vector<BoundaryCurve2D> curves;
    bool isHole = false;
};

// if (const auto line = std::get_if<LineSegment2D>(&curve)) {
//     return Point2D{.x = line->a.x + t * (line->b.x - line->a.x),
//                    .y = line->a.y + t * (line->b.y - line->a.y)};
// }