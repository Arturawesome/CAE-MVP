#include "TriangleP2.h"


TriangleP2::TriangleP2(const Point2D &p1, const Point2D &p2, const Point2D &p3, int elementID) {
    CalcArea(p1, p2, p3, elementID);

    center_.x = (p1.x + p2.x + p3.x) / 3.0;
    center_.y = (p1.y + p2.y + p3.y) / 3.0;

    CreateBarycentric(p1, p2, p3);
}


double TriangleP2::Area() const { return area_; }

Point2D TriangleP2::Center() const { return center_; }

Eigen::Matrix<double, 2, 6> TriangleP2::Grad(const Point2D &point) const {
    Eigen::Matrix<double, 2, 6> grad;
    Eigen::Vector3d coords;
    coords << 1.0, point.x, point.y;

    Eigen::Vector3d barCoords = barCoefs_.transpose() * coords;

    const auto g1 = barGrad_.col(0); //.eval() -нужен для полного копирования,
                                     // нет записи в г1 -> только чтение данных
                                     // г1 не изменяетс, значит евал не нуэен
    const auto g2 = barGrad_.col(1);
    const auto g3 = barGrad_.col(2);

    grad.col(0) = (4 * barCoords[0] - 1) * g1;
    grad.col(1) = (4 * barCoords[1] - 1) * g2;
    grad.col(2) = (4 * barCoords[2] - 1) * g3;

    grad.col(3) = 4 * (barCoords[0] * g2 + barCoords[1] * g1);
    grad.col(4) = 4 * (barCoords[1] * g3 + barCoords[2] * g2);
    grad.col(5) = 4 * (barCoords[2] * g1 + barCoords[0] * g3);

    return grad;
}

Eigen::Matrix<double, 6, 1> TriangleP2::Shape(const Point2D &point) const {
    Eigen::Vector3d coords;
    coords << 1.0, point.x, point.y;
    Eigen::Vector3d barCoords = barCoefs_.transpose() * coords;
    Eigen::Matrix<double, 6, 1> shape;

    shape(0) = barCoords[0] * (2 * barCoords[0] - 1);
    shape(1) = barCoords[1] * (2 * barCoords[1] - 1);
    shape(2) = barCoords[2] * (2 * barCoords[2] - 1);

    shape(3) = 4 * barCoords[0] * barCoords[1];
    shape(4) = 4 * barCoords[1] * barCoords[2];
    shape(5) = 4 * barCoords[2] * barCoords[0];

    return shape;
}

void TriangleP2::CalcArea(const Point2D &p1, const Point2D &p2,
                          const Point2D &p3, int elementID) {
    const auto isFinite = [](const Point2D &p) {
        return std::isfinite(p.x) && std::isfinite(p.y);
    };

    if (!isFinite(p1) || !isFinite(p2) || !isFinite(p3)) {
        throw std::invalid_argument(
            "TriangleP2: non-finite coordinates in element " +
            std::to_string(elementID));
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

    if (!std::isfinite(maxLength) || maxLength <= 0.0 ||
        !std::isfinite(doubleArea) ||
        std::abs(doubleArea) / maxLength / maxLength <= kMinShapeRatio) {

        throw std::runtime_error("TriangleP2: degenerate triangle, element " +
                                 std::to_string(elementID));
    }

    area_ = 0.5 * std::abs(doubleArea);
}

void TriangleP2::CreateBarycentric(const Point2D &p1, const Point2D &p2,
                                   const Point2D &p3) {
    Eigen::Matrix3d triangle; 

    triangle << 1, p1.x, p1.y, 1, p2.x, p2.y, 1, p3.x, p3.y;
    // 1 x1 y1
    // 1 x2 y1
    // ...

    barCoefs_ = triangle.inverse();
    // 1 1 1
    // X1 X2 X3
    // ...

    // <row_size, col_size> (strat_row, start_col)  <>() -- static
    barGrad_ = barCoefs_.block<2, 3>(1, 0);
}