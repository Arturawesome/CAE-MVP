#include "TriangleP1.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>


double TriangleP1::Area() const { return area_; }
Point2D TriangleP1::Center() const { return center_; }
const Eigen::Matrix<double, 2, 3> &TriangleP1::Grad() const { return grad_; }

TriangleP1::TriangleP1(const Mesh& mesh, int elementID) {
    const auto& element = mesh.ElementById(elementID);

    const auto& p1 = mesh.NodeById(element.nodes_id[0]).position;
    const auto& p2 = mesh.NodeById(element.nodes_id[1]).position;
    const auto& p3 = mesh.NodeById(element.nodes_id[2]).position;

    const auto isFinite = [](const Point2D& p) {
        return std::isfinite(p.x) && std::isfinite(p.y);
    };

    if (!isFinite(p1) || !isFinite(p2) || !isFinite(p3)) {
        throw std::invalid_argument(
            "TriangleP1: non-finite coordinates in element " +
            std::to_string(elementID)
        );
    }

    const double dx12 = p2.x - p1.x;
    const double dy12 = p2.y - p1.y;

    const double dx13 = p3.x - p1.x;
    const double dy13 = p3.y - p1.y;

    const double dx23 = p3.x - p2.x;
    const double dy23 = p3.y - p2.y;

    const double l12 = std::hypot(dx12, dy12);
    const double l13 = std::hypot(dx13, dy13);
    const double l23 = std::hypot(dx23, dy23);

    const double maxLength = std::max({l12, l13, l23});


    const double doubleArea = dx12 * dy13 - dy12 * dx13;

    constexpr double kMinShapeRatio = 1e-12;

    if (!std::isfinite(maxLength) ||
        maxLength <= 0.0 ||
        !std::isfinite(doubleArea) ||
        std::abs(doubleArea) / maxLength / maxLength <= kMinShapeRatio) {

        throw std::runtime_error(
            "TriangleP1: degenerate triangle, element " +
            std::to_string(elementID)
        );
    }

    area_ = 0.5 * std::abs(doubleArea);

    center_.x = (p1.x + p2.x + p3.x) / 3.0;
    center_.y = (p1.y + p2.y + p3.y) / 3.0;

    CreateGradMatrix(p1, p2, p3);
}

void TriangleP1::CreateGradMatrix(const Point2D &p1, const Point2D &p2,
                                  const Point2D &p3) {
    Eigen::MatrixXd triangle = Eigen::MatrixXd::Zero(3, 3);

    triangle(0, 0) = 1;
    triangle(0, 1) = p1.x;
    triangle(0, 2) = p1.y;

    triangle(1, 0) = 1;
    triangle(1, 1) = p2.x;
    triangle(1, 2) = p2.y;

    triangle(2, 0) = 1;
    triangle(2, 1) = p3.x;
    triangle(2, 2) = p3.y;

    const Eigen::Matrix3d coefs = triangle.inverse();
    shapeCoefs_ = coefs.row(0).transpose();
    grad_ = coefs.block(1, 0, 2, 3);
}

Eigen::Vector3d TriangleP1:: Shape(const Point2D& p) const {
    return shapeCoefs_
         + p.x * grad_.row(0).transpose()
         + p.y * grad_.row(1).transpose();
}