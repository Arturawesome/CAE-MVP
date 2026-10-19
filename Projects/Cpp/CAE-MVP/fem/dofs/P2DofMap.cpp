

#include "P2DofMap.h"
#include "mesh/MeshTypes2D.h"
#include <unordered_map>
#include <utility>

P2DofMap::P2DofMap(const Mesh &mesh) {
    p2Nodes_.reserve(2 * (mesh.Nodes().size() + 1));
    elementNodesId_.reserve(mesh.NumEl());// = std::vector<std::array<int, 6>>(mesh_.NumEl());
    p2Nodes_ = mesh.Nodes();
    CreateMiddlePoints(mesh);
}

int P2DofMap::NumP2Nodes() const { return p2Nodes_.size(); }

const Node &P2DofMap::P2NodeById(int p2NodeId) const {
    return p2Nodes_.at(p2NodeId);
}
const std::array<int, 6>& P2DofMap::ElementNodes(int elementId) const {
    return elementNodesId_.at(elementId);
}

void P2DofMap::CreateMiddlePoints(const Mesh &mesh) {
    using Edge = std::pair<int, int>;
    struct PairHash {
        std::size_t operator()(const Edge &p) const {
            auto h1 = std::hash<int>{}(p.first);
            auto h2 = std::hash<int>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
    std::unordered_map<Edge, int, PairHash> edgeToNode_;
    
    std::array<int, 6> localNodes;
    for (const auto &element : mesh.Elements()) {
        std::array<Edge, 3> edges = {
            Edge{element.nodes_id[0], element.nodes_id[1]},
            Edge{element.nodes_id[1], element.nodes_id[2]},
            Edge{element.nodes_id[2], element.nodes_id[0]},
        };
        localNodes[0] = element.nodes_id[0];
        localNodes[1] = element.nodes_id[1];
        localNodes[2] = element.nodes_id[2];
        for (auto edge : edges) {
            int id1 = edge.first;
            int id2 = edge.second;
            if (id1 > id2) {
                std::swap(id1, id2);
            }
            if (edgeToNode_.contains({id1, id2}))
                continue;
            edgeToNode_[{id1, id2}] = p2Nodes_.size();
            const Node n1 = mesh.NodeById(id1);
            const Node n2 = mesh.NodeById(id2);

            p2Nodes_.push_back(Node{
                .position = Point2D{.x = (n1.position.x + n2.position.x) / 2,
                                    .y = (n1.position.y + n2.position.y) / 2}});
        }
        int count = 3;
        for (auto edge : edges) {
            int id1 = edge.first;
            int id2 = edge.second;
            if (id1 > id2) {
                std::swap(id1, id2);
            }
            localNodes[count] = edgeToNode_[{id1, id2}];
            count++;
        }
        elementNodesId_.push_back(localNodes);
    }
}
