#pragma once

class Geometry {
    public:
        virtual ~Geometry() = default;
        virtual bool contains(double x, double y) const = 0;
};

