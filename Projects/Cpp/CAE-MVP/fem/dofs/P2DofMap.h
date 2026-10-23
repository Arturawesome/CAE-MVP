#pragma once
#include "mesh/Mesh2D.h"
#include <array>
#include <vector>

class P2DofMap {
    private:

        // start with mesh node, and after median of edges
        std::vector<Node> p2Nodes_; 
        std::vector<std::array<int, 6>> elementNodesId_;
    
    private:
        void CreateMiddlePoints(const Mesh &mesh);
    public:
        P2DofMap(const Mesh& mesh);
        int NumP2Nodes() const;
        int NumP2Elements() const;
        const Node& P2NodeById(int p2NodeId) const;
        const std::array<int, 6>& NodesIdByElementId(int elementId) const;
        
};