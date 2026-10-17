#pragma once
#include "geometry/Geometry2D.h"
#include "mesh/Mesh2D.h"

class TriangularMesher2D {
private:
    int NumElements_ = 10;
    const  Geometry2D& figure_;
    void generateBoundaryEdges(Mesh &m);

public:
    TriangularMesher2D(const  Geometry2D& figure);
    ~TriangularMesher2D();
    Mesh GenerateMesh() const;
};

