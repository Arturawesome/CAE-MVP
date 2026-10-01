#pragma once

#include "Geometry2D.h"





struct Node {
    int id;
    Point2D position;
};

// EDJE OF ELEMENT on boundary SIDE
struct BoundaryEdge {
    std::pair<int, int> nodes_id;
    BoundaryId boundaryId;
};

// triangle element: id, area and nodes
struct Element {
    int id;
    std::array<int, 3> nodes_id;
};