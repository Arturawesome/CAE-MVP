#pragma once
#include "BoundaryId.h"
#include "MeshTypes2D.h"
#include "geometry/GeometryTypes2D.h"
#include <cmath>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
namespace mesh {
class Mesh {
  private:
    std::vector<Node> nodes_;
    std::vector<Element> elements_;
    std::vector<BoundaryEdge> boundaryEdges_;

  public:
    Mesh() {}
    void AddNode(Node node);
    void AddElement(Element element);
    void AddBoundaryEdge(BoundaryEdge edge);
    void CreateBoundaryEdges(BoundaryId defaultId = DefaultBoundaryId);

    const std::vector<Node> &Nodes() const;
    const std::vector<Element> &Elements() const;
    const std::vector<BoundaryEdge> &BoundaryEdges() const;

    const Node &NodeById(int id) const;
    const Element &ElementById(int id) const;

    int NodeIdByBoundaryEdge(const BoundaryEdge &edge) const;

    int NumNodes() const;
    int NumEl() const;
    int NumBoundEdges() const;

    int NodeIdFromEl(int elId, std::size_t i) const;
    int NumNodesInEl(std::size_t elId) const;

    geometry::Point2D OutwardNormal(const BoundaryEdge &edge) const;

    friend std::ostream &operator<<(std::ostream &stream, const Mesh &m);
};
} // namespace mesh