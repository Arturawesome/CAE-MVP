#pragma once
#include <fstream>
#include <string>
#include <utility>
#include <vector>

struct Node {
    int id;
    double x;
    double y;
};
std::ostream &operator<<(std::ostream &stream, const Node &node);

// triangle element: id, area and nodes
struct Element {
    int id;
    double s;
    std::vector<int> nodes_id = std::vector<int>(3, 0);
};

std::ostream &operator<<(std::ostream &stream, const Element &element);

// marker is !!!NOT THE BOUND CONDIONOS!!!
// marker is the marker of are: left boundary EDGE, right boundary EDGE
struct BoundaryEdge {
    std::pair<int, int> nodes_id;
    int marker;
};

class Mesh {
  public:
    std::vector<Node> nodes;
    std::vector<Element> elements;
    std::vector<BoundaryEdge> boundaryEdges;

  public:
    int NumNodes() const;
    int NumEl() const;
    int NumBoundEdges() const;
    int NodeIdFromEl(int elId, std::size_t i) const;
    int NumNodesInEl(std::size_t elId) const;

    const Node &GetNode(int id) const;
    const Element &GetEl(int id) const;
    int GetMarker(int i) const;
    const std::vector<BoundaryEdge> &GetBoundaryEdge() const;

    void saveToSTL(const std::string &filename) const;
    void saveToVTK(const std::string &filename) const;
    bool hasBoundaryMarker(int marker) const;
};