#pragma once
#include <iostream>
class Geometry {
  public:
    virtual ~Geometry() = default;
    virtual bool contains(double x, double y) const = 0;
    virtual double getHeight() const = 0;
    virtual double getWidth() const = 0;
};
