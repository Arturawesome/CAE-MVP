#include "Mesh2D.h"

#include <fstream>
#include <stdexcept>

std::ostream &operator<<(std::ostream &stream, const Mesh &m) {
    for (const auto node : m.nodes_) {
        stream << "{x=" << node.position.x
               << "; y=" << node.position.y << "}\n";
    }

    stream << "\n\n";
    int counter = 0;
    for (const auto el : m.elements_) {
        stream << "{";
        for (const auto node_is : el.nodes_id) {
            stream << node_is << ", ";
        }
        stream << "}\n ";
        if (counter++ > 5)
            break;
    }

    return stream;
}



std::ostream &operator<<(std::ostream &stream, const Element &element) {
    stream << "{n_id = ";
    for (const auto node_is : element.nodes_id) {
        stream << node_is << ", ";
    }
    stream << "}\n ";
    return stream;
}

const Node &Mesh::NodeById(int id) const { return nodes_[id]; }
const Element &Mesh::ElementById(int id) const { return elements_[id]; }

int Mesh::NumNodesInEl(std::size_t elId) const {
    return elements_[elId].nodes_id.size();
}

int Mesh::NodeIdFromEl(int elId, std::size_t i) const {
    if (elId < 0 || static_cast<std::size_t>(elId) >= elements_.size()) {
        throw std::out_of_range("Mesh::NodeIdFromEl: invalid element id");
    }

    if (i < elements_[elId].nodes_id.size()) {
        return elements_[elId].nodes_id[i];
    }
    throw std::invalid_argument("index of nodes_id is not correct\n");
}

const std::vector<BoundaryEdge> &Mesh::BoundaryEdges() const {
    return boundaryEdges_;
}
const std::vector<Node> &Mesh::Nodes() const { return nodes_; }
const std::vector<Element> &Mesh::Elements() const { return elements_; }

int Mesh::GetMarker(int i) const { return boundaryEdges_[i].boundaryId; }

int Mesh::NumNodes() const { return nodes_.size(); }
int Mesh::NumEl() const { return elements_.size(); }
int Mesh::NumBoundEdges() const { return boundaryEdges_.size(); }

bool Mesh::HasBoundaryMarker(int marker) const {
    for (const BoundaryEdge &be : boundaryEdges_) {
        if (be.boundaryId == marker) {
            return true;
        }
    }
    return false;
}

void Mesh::AddNode(Node node) { nodes_.push_back(node); }
void Mesh::AddElement(Element element) { elements_.push_back(element); }
void Mesh::AddBoundaryEdge(BoundaryEdge edge) {
    boundaryEdges_.push_back(edge);
}