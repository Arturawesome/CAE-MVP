#include "TriangularMesher2D.h"

TriangularMesher2D::TriangularMesher2D(const  Geometry2D& figure):figure_(figure) {} 

TriangularMesher2D::~TriangularMesher2D() {}

Mesh TriangularMesher2D::GenerateMesh() const {
    const std::vector<Contour2D> contours = figure_.Contours();
    Mesh mesh;
    for(const auto& boundary: contours){
        for(const auto& curve: boundary.curves){
            double L = curve.Lenght();
            double dl = L / NumElements;
            
        }
    }
    return mesh;
}






































