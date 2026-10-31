#include "TriangularMesher2D.h"
namespace mesh {
TriangularMesher2D::TriangularMesher2D(double targetEdgeLength,
                                       const geometry::Geometry2D &figure)
    : targetEdgeLength_(targetEdgeLength), figure_(figure) {}

TriangularMesher2D::~TriangularMesher2D() {}

Mesh TriangularMesher2D::GenerateMesh() {
    const std::vector<geometry::Contour2D> contours = figure_.Contours();
    Mesh mesh;
    for (const auto &boundary : contours) {
        for (const auto &curve : boundary.curves) {
        }
    }
    return mesh;
}

void TriangularMesher2D::GeneratePointsInFigure() {
    const std::vector<geometry::Contour2D> contours = figure_.Contours();
    for (const auto &boundary : contours) {
        for (const auto &curve : boundary.curves) {
        }
    }
}
} // namespace mesh