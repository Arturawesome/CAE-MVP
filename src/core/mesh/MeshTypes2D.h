#pragma once
#include "BoundaryId.h"
#include "geometry/GeometryTypes2D.h"
#include <array>
#include <utility>
namespace mesh {

struct Node {
    geometry::Point2D position;
};

// EDJE OF ELEMENT on boundary SIDE
struct BoundaryEdge {
    std::pair<int, int> nodes_id;
    BoundaryId boundaryId;
    int elementId = -1;
};

// triangle element: id, area and nodes
struct Element {
    std::array<int, 3> nodes_id;
};
} // namespace mesh