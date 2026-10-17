#pragma once
#include "mesh/Mesh2D.h"


class P2DofMap {
    private:
        const Mesh& mesh_;

        // start with mesh node, and after median of edges
        std::vector<Node> p2nNodes_; 
    
    private:
        void CreateMiddlePoints();
    public:
        P2DofMap(const Mesh& mesh);

};