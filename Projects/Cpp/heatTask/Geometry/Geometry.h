#pragma once
#include <iostream>
class Geometry
{
public:
    virtual ~Geometry() = default;
    virtual bool contains(double x, double y) const = 0;
};
