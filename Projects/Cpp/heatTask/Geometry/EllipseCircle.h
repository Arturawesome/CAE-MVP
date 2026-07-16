#pragma once
#include "Geometry.h"
class EllipseCircle : public Geometry {
  private:
    double r = 0;
    double a = 1;
    double b = 1;

  public:
    EllipseCircle(double r_, double a_, double b_) : r(r_), a(a_), b(b_) {}
    bool contains(double x, double y) const override {
        if (a == b) {
            return x * x + y * y <= r * r;
        }
        return x * x / (a * a) + y * y / (b * b) <= 1;
        
    }
    double getHeight() const override{
        if(a == b){
            return r;
        }
        return b;
    }
    double getWidth() const override{
        if(a == b){
            return r;
        }
        return a;
    }
};