#pragma once

#include "Geometry.h"

class Rectangle : public Geometry {
  private:
    double height;
    double width;

  public:
    Rectangle(double h, double w) : height(h), width(w) {
        std::cout << "Rectangle: Rectangle\n";
    }
    double getHeight() const {
        std::cout << "Rectangle: getHeight\n";
        return height;
    }
    double getWidth() const {
        std::cout << "Rectangle: getWidth\n";
        return width;
    }
    bool contains(double x, double y) const override {
        std::cout << "Rectangle: contains\n";
        bool is_contains = (x >= 0 && x <= width && y >= 0 && y <= height);
        return is_contains;
    }
};