#include "Mesh2D.h"

#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
namespace mesh {
std::ostream &operator<<(std::ostream &stream, const Mesh &m) {
    for (const auto node : m.nodes_) {
        stream << "{x=" << node.position.x << "; y=" << node.position.y
               << "}\n";
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
void Mesh::CreateBoundaryEdges(BoundaryId defaultId) {

    using Edge = std::pair<int, int>;

    struct EdgeInfo {
        int elementId = -1;
        int count = 0;
    };
    struct PairIntHash {
        std::size_t operator()(const Edge &edge) const {
            auto h1 = std::hash<int>{}(edge.first);
            auto h2 = std::hash<int>{}(edge.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    std::unordered_map<Edge, BoundaryId, PairIntHash> boundaryMarkers;

    for (const BoundaryEdge &edge : boundaryEdges_) {
        auto [a, b] = edge.nodes_id;

        if (a > b) {
            std::swap(a, b);
        }

        boundaryMarkers[Edge{a, b}] = edge.boundaryId;
    }
    std::unordered_map<Edge, EdgeInfo, PairIntHash> freakEdges;
    for (int i = 0; i < elements_.size(); ++i) {
        const auto &element = elements_[i];

        const auto &nodes_id = element.nodes_id;
        std::array<Edge, 3> edges = {
            Edge{nodes_id[0], nodes_id[1]},
            Edge{nodes_id[1], nodes_id[2]},
            Edge{nodes_id[2], nodes_id[0]},
        };
        for (auto [a, b] : edges) {
            if (a > b) {
                std::swap(a, b);
            }
            auto [it, inserted] =
                freakEdges.try_emplace(Edge{a, b}, EdgeInfo{i, 0});
            ++it->second.count;
            if (it->second.count > 2) {
                throw std::runtime_error("Mesh: non-manifold edge");
            }
        }
    }

    std::vector<BoundaryEdge> newEdges;
    for (const auto &[edge, info] : freakEdges) {
        if (info.count != 1) {
            continue;
        }

        BoundaryId boundaryId = defaultId;

        const auto it = boundaryMarkers.find(edge);
        if (it != boundaryMarkers.end()) {
            boundaryId = it->second;
        }

        newEdges.push_back(BoundaryEdge{.nodes_id = edge,
                                        .boundaryId = boundaryId,
                                        .elementId = info.elementId});
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

const std::vector<Node> &Mesh::Nodes() const { return nodes_; }
const std::vector<Element> &Mesh::Elements() const { return elements_; }

int Mesh::NumNodes() const { return nodes_.size(); }
int Mesh::NumEl() const { return elements_.size(); }
int Mesh::NumBoundEdges() const { return boundaryEdges_.size(); }

const Node &Mesh::NodeById(int id) const { return nodes_[id]; }
const Element &Mesh::ElementById(int id) const { return elements_[id]; }

geometry::Point2D Mesh::OutwardNormal(const BoundaryEdge &edge) const {
    const int id1 = edge.nodes_id.first;
    const int id2 = edge.nodes_id.second;

    if (id1 == id2) {
        throw std::invalid_argument("Mesh::OutwardNormal: degenerate edge");
    }

    const geometry::Point2D &a = NodeById(id1).position;
    const geometry::Point2D &b = NodeById(id2).position;

    const double dx = b.x - a.x;
    const double dy = b.y - a.y;

    const double length = std::hypot(dx, dy);

    if (!std::isfinite(length) || length <= 0.0) {
        throw std::invalid_argument("Mesh::OutwardNormal: invalid edge length");
    }

    geometry::Point2D normal{dy / length, -dx / length};

    // Найти третий узел  треугольника
    bool hasId1 = false;
    bool hasId2 = false;
    int oppositeNodeId = -1;

    for (int nodeId : elements_[edge.elementId].nodes_id) {
        if (nodeId == id1) {
            hasId1 = true;
        }
        else if (nodeId == id2) {
            hasId2 = true;
        }
        else {
            oppositeNodeId = nodeId;
        }
    }

    if (!hasId1 || !hasId2 || oppositeNodeId < 0) {
        throw std::runtime_error(
            "Mesh::OutwardNormal: invalid edge-element adjacency");
    }

    const geometry::Point2D &c = NodeById(oppositeNodeId).position;

    const double mx = 0.5 * (a.x + b.x);
    const double my = 0.5 * (a.y + b.y);

    // Вектор от середины ребра внутрь треугольника
    const double inwardX = c.x - mx;
    const double inwardY = c.y - my;

    const double dot = normal.x * inwardX + normal.y * inwardY;

    if (!std::isfinite(dot) || std::abs(dot) <= 1e-14 * length) {
        throw std::runtime_error(
            "Mesh::OutwardNormal: degenerate adjacent triangle");
    }

    // Если нормаль направлена внутрь, разворачиваем
    if (dot > 0.0) {
        normal.x = -normal.x;
        normal.y = -normal.y;
    }

    return normal;
}

int Mesh::NodeIdByBoundaryEdge(const BoundaryEdge &edge) const {
    const int id1 = edge.nodes_id.first;
    const int id2 = edge.nodes_id.second;

    if (id1 == id2) {
        throw std::invalid_argument("Mesh::OutwardNormal: degenerate edge");
    }

    // Найти третий узел  треугольника
    bool hasId1 = false;
    bool hasId2 = false;
    int oppositeNodeId = -1;

    for (int nodeId : elements_[edge.elementId].nodes_id) {
        if (nodeId == id1) {
            hasId1 = true;
        }
        else if (nodeId == id2) {
            hasId2 = true;
        }
        else {
            oppositeNodeId = nodeId;
        }
    }

    if (!hasId1 || !hasId2 || oppositeNodeId < 0) {
        throw std::runtime_error(
            "Mesh::OutwardNormal: invalid edge-element adjacency");
    }

    return oppositeNodeId;
}

void Mesh::AddNode(Node node) { nodes_.push_back(node); }
void Mesh::AddElement(Element element) { elements_.push_back(element); }
void Mesh::AddBoundaryEdge(BoundaryEdge edge) {
    boundaryEdges_.push_back(edge);
}
} // namespace mesh