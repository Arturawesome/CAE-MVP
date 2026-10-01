#pragma once
#include <vector>


struct Point2D {
    double x = 0.0;
    double y = 0.0;
};

using BoundaryId = int;

// set the boundary segment (ALL side of figure, not NODE or edje of element. ALL SIDE)
struct Segment2D {
    Point2D a;
    Point2D b;
    BoundaryId boundaryId;
};

// vector of boundaries in our geometry
struct Contour2D {
    std::vector<Segment2D> segments;
    bool isHole = false;
};


