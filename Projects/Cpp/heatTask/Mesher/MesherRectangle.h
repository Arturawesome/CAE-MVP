#include "../Geometry/Rectangle.h"
#include "../Mesh/Mesh.h"
#include <cmath>
#include <iomanip>
#include <iostream>

enum class RectangleBoundaryMarker { Bottom = 1, Right = 2, Top = 3, Left = 4 };

class MesherRectangle {
  private:
    int nx = 2; // numbers of NODES
    int ny = 2; // numbers of NODES
    void createElement(int n0, int n1, int n2, int id_el, double dx, double dy,
                       Element &el);
    void generateBoundaryEdges(Mesh &m);
    int getNodeId(int i, int j);

  public:
    MesherRectangle(int nx_, int ny_);
    Mesh generateMesh(const Rectangle &r);
};