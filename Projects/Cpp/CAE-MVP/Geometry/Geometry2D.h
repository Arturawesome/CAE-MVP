#pragma once
#include "GeometryTypes2D.h"

namespace geometry {
class Geometry2D {
  public:
    virtual ~Geometry2D() = default;
    virtual bool Contains(const Point2D &p) const = 0;

    virtual std::vector<Contour2D> Contours() const = 0;
};
} // namespace geometry