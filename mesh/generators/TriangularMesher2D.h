#pragma once
#include "geometry/Geometry2D.h"
#include "mesh/Mesh2D.h"
#include <vector>
namespace mesh {
class TriangularMesher2D {
  private:
    double targetEdgeLength_ = 1;
    const geometry::Geometry2D &figure_;
    std::vector<geometry::Point2D> points_;
    void generateBoundaryEdges(Mesh &m);
    void GeneratePointsInFigure();

  public:
    TriangularMesher2D(double targetEdgeLength,
                       const geometry::Geometry2D &figure);
    ~TriangularMesher2D();
    Mesh GenerateMesh();
};

} // namespace mesh