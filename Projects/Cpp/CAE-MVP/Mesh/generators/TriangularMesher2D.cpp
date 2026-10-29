#include "TriangularMesher2D.h"

TriangularMesher2D::TriangularMesher2D(double targetEdgeLength,
                                       const Geometry2D &figure)
    : targetEdgeLength_(targetEdgeLength), figure_(figure) {}

TriangularMesher2D::~TriangularMesher2D() {}

Mesh TriangularMesher2D::GenerateMesh()  {
    const std::vector<Contour2D> contours = figure_.Contours();
    Mesh mesh;
    for (const auto &boundary : contours) {
        for (const auto &curve : boundary.curves) {
        }
    }
    return mesh;
}

void TriangularMesher2D::GeneratePointsInFigure() {
    const std::vector<Contour2D> contours = figure_.Contours();
     for (const auto &boundary : contours) {
        for (const auto &curve : boundary.curves) {
        }
    }
}