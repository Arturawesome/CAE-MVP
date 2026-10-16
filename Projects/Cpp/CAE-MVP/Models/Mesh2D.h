#pragma once
#include <array>
#include <utility>
#include "Geometry2D.h"
#include "BoundaryId.h"

struct Node {
    Point2D position;
};

// EDJE OF ELEMENT on boundary SIDE
struct BoundaryEdge {
    std::pair<int, int> nodes_id;
    BoundaryId boundaryId;
};

// triangle element: id, area and nodes
struct Element {
    std::array<int, 3> nodes_id;
};