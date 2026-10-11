#include "TriangleP1.h"

TriangleP1::TriangleP1(const Mesh &mesh, int elementID) {
    const auto &element = mesh.ElementById(elementID);
    const auto &node1 = mesh.NodeById(element.nodes_id[0]);
    const auto &node2 = mesh.NodeById(element.nodes_id[1]);
    const auto &node3 = mesh.NodeById(element.nodes_id[2]);

    center_.x = (node1.position.x + node2.position.x + node3.position.x) / 3;
    center_.y = (node1.position.y + node2.position.y + node3.position.y) / 3;

    area_ = node1.position.x * (node2.position.y - node3.position.y) +
            node2.position.x * (node3.position.y - node1.position.y) +
            node3.position.x * (node1.position.y - node2.position.y);
    CreateGradMatrix(node1.position, node2.position, node3.position);
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

    grad_ = triangle.inverse().block(1, 0, 2, 3);
}
