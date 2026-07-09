#pragma once

#include "Geometry.h"

class Rectangle: public Geometry {
    private:
        double height;
        double widht;
    public:
        Rectangle(double h, double w): height(h), widht(w) {std::cout<<"Rectangle: Rectangle\n";}
        double getHeight() const {
            std::cout<<"Rectangle: getHeight\n";
            return height;
        }
        double getWidth() const {
            std::cout<<"Rectangle: getWidth\n";
            return widht;
        }
        bool contains(double x, double y) const override {
            std::cout<<"Rectangle: contains\n";
            bool is_contains = (x >= 0 && x <= widht &&
                                y >= 0 && y <= height);
            return is_contains;
        }
};