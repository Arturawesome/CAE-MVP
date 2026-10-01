#pragma once
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "../models/Geometry2D.h"
#include "../models/Mesh2D.h"

class Mesh {
  private:
    std::vector<Node> nodes_;
    std::vector<Element> elements_;
    std::vector<BoundaryEdge> boundaryEdges_;

  public:

    Mesh() {
        nodes_.reserve(1000);
        elements_.reserve(1000);
        boundaryEdges_.reserve(1000);
    }
    void AddNode(Node node);
    void AddElement(Element element);
    void AddBoundaryEdge(BoundaryEdge edge);

    const std::vector<Node> &Nodes() const;
    const std::vector<Element> &Elements() const;
    const std::vector<BoundaryEdge> &BoundaryEdges() const;

    const Node &NodeById(int id) const;
    const Element &ElementById(int id) const;

    int NumNodes() const;
    int NumEl() const;
    int NumBoundEdges() const;

    int NodeIdFromEl(int elId, std::size_t i) const;
    int NumNodesInEl(std::size_t elId) const;

    int GetMarker(int i) const;
    bool HasBoundaryMarker(int marker) const;


    friend std::ostream &operator<<(std::ostream &stream, const Mesh &m);
    
};
