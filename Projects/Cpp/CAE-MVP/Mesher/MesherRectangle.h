#include "../Geometry/Rectangle.h"
#include "../Mesh/Mesh2D.h"
#include <cmath>
#include <iomanip>
#include <iostream>



class MesherRectangle {
  private:
    int nx = 2; // numbers of NODES
    int ny = 2; // numbers of NODES

    void generateBoundaryEdges(Mesh &m);
    int getNodeId(int i, int j);

  public:
    MesherRectangle(int nx_, int ny_);
    Mesh generateMesh(const Rectangle &r);
};