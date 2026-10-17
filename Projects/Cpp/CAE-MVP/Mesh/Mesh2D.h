#pragma once
#include <fstream>
#include <string>
#include <utility>
#include <vector>


#include "../models/Geometry2D.h"
#include "../models/Mesh2D.h"
#include "../models/BoundaryId.h"

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

    int NumNodes() const;
    int NumEl() const;
    int NumBoundEdges() const;

    int NodeIdFromEl(int elId, std::size_t i) const;
    int NumNodesInEl(std::size_t elId) const;




    friend std::ostream &operator<<(std::ostream &stream, const Mesh &m);
    
};
