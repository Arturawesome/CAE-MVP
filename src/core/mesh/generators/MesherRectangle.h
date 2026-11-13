#pragma once

#include "geometry/Rectangle.h"
#include "mesh/Mesh2D.h"
#include <cmath>
#include <iomanip>
#include <iostream>

namespace mesh {
class MesherRectangle {
  private:
    int nx_ = 2; // numbers of NODES
    int ny_ = 2; // numbers of NODES

    void generateBoundaryEdges(Mesh &m);
    int getNodeId(int i, int j);

  public:
    MesherRectangle(int nx, int ny);
    Mesh generateMesh(const geometry::Rectangle &r);
};
} // namespace mesh