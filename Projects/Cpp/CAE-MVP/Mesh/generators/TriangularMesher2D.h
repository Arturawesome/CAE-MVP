#pragma once
#include "geometry/Geometry2D.h"
#include "mesh/Mesh2D.h"
#include <vector>

class TriangularMesher2D {
private:
    double targetEdgeLength_ = 1;
    const  Geometry2D& figure_;
    std::vector<Point2D> points_;
    void generateBoundaryEdges(Mesh &m);
    void GeneratePointsInFigure();

public:
    TriangularMesher2D(double targetEdgeLength, const  Geometry2D& figure);
    ~TriangularMesher2D();
    Mesh GenerateMesh() ;
};

