#pragma once
#include "mesh/Mesh2D.h"
#include <array>
#include <vector>

using Edge = std::pair<int, int>;
struct PairHash {
    std::size_t operator()(const Edge &p) const {
        auto h1 = std::hash<int>{}(p.first);
        auto h2 = std::hash<int>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};

class P2DofMap {
  private:
    // start with mesh node, and after median of edges
    std::vector<Node> p2Nodes_;
    std::vector<std::array<int, 6>> elementNodesId_;
    std::unordered_map<Edge, int, PairHash> edgeToNode_;

  private:
    void CreateMiddlePoints(const Mesh &mesh);

  public:
    P2DofMap(const Mesh &mesh);
    int NumP2Nodes() const;
    int NumP2Elements() const;
    int MiddleNodeId(int nodeId1, int nodeId2) const;
    const Node &P2NodeById(int p2NodeId) const;
    const std::array<int, 6> &NodesIdByElementId(int elementId) const;
};