#pragma once
#include "../models/Geometry2D.h"
class Geometry2D {
  public:
    virtual ~Geometry2D() = default;
    virtual bool Contains(const Point2D& p) const = 0;

    virtual std::vector<Contour2D> Contours() const = 0;
};
