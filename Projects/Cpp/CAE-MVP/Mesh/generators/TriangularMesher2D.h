#pragma once
#include "../../geometry/Geometry2D.h"
#include "../Mesh2D.h"

class TriangularMesher2D {
private:
    int NumElements = 10;
    void generateBoundaryEdges(Mesh &m);
    
    const  Geometry2D& figure_;
public:
    TriangularMesher2D(const  Geometry2D& figure);
    ~TriangularMesher2D();
    Mesh GenerateMesh() const;
};

