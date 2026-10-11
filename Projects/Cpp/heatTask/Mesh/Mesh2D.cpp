#include "Mesh2D.h"

#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <array>
#include <iostream>

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
void Mesh::CreateBoundaryEdges() {
    using Edge = std::pair<int, int>;
    struct PairIntHash {
        std::size_t operator()(const Edge& nodesID) const {
            auto h1 = std::hash<int>{}(nodesID.first);
            auto h2 = std::hash<int>{}(nodesID.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    std::unordered_map<Edge, int, PairIntHash> freakEdges;
    for(const auto& element: elements_){
        const auto& nodes_id = element.nodes_id;
        std::array<Edge, 3> edges  = {
            Edge{nodes_id[0], nodes_id[1]},
            Edge{nodes_id[1], nodes_id[2]},
            Edge{nodes_id[2], nodes_id[0]},
        };
        for(auto [a, b]:  edges) {
             if (a > b) {
                std::swap(a, b);
            }
            freakEdges[{a, b}]++;
        }   
    }

    std::vector<BoundaryEdge> newEdges;

    for(const auto&[edje, freak]: freakEdges){
        if(freak > 2) {
            std::cerr <<"Incorrect mesh: edje freak more then 2";
            throw std::runtime_error("Incorrect mesh: edje freak more then 2");
        } 
        if (freak == 1) {
            newEdges.push_back(BoundaryEdge{.nodes_id = edje, .boundaryId = 1});
        }
    }
    boundaryEdges_ = std::move(newEdges);
}


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


bool Mesh::HasBoundaryMarker(int marker) const {
    for (const BoundaryEdge &be : boundaryEdges_) {
        if (be.boundaryId == marker) {
            return true;
        }
    }
    return false;
}

const std::vector<Node> &Mesh::Nodes() const { return nodes_; }
const std::vector<Element> &Mesh::Elements() const { return elements_; }
int Mesh::GetMarker(int i) const { return boundaryEdges_[i].boundaryId; }

int Mesh::NumNodes() const { return nodes_.size(); }
int Mesh::NumEl() const { return elements_.size(); }
int Mesh::NumBoundEdges() const { return boundaryEdges_.size(); }

const Node &Mesh::NodeById(int id) const { return nodes_[id]; }
const Element &Mesh::ElementById(int id) const { return elements_[id]; }



void Mesh::AddNode(Node node) { nodes_.push_back(node); }
void Mesh::AddElement(Element element) { elements_.push_back(element); }
void Mesh::AddBoundaryEdge(BoundaryEdge edge) {
    boundaryEdges_.push_back(edge);
}