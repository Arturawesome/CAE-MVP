#pragma once

#include "P2DofMap.h"
#include <unordered_set>
#include <utility>

P2DofMap::P2DofMap(const Mesh &mesh) : mesh_(mesh) {
    p2nNodes_.reserve(2*(mesh_.Nodes().size() + 1));
    p2nNodes_ = mesh_.Nodes();

    CreateMiddlePoints();
}

void P2DofMap::CreateMiddlePoints() {
    struct PairHash {
        std::size_t operator()(const std::pair<int, int>& p) const {
            auto h1 = std::hash<int>{}(p.first);
            auto h2 = std::hash<int>{}(p.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
    std::unordered_set<std::pair<int, int>,PairHash> hash;

}
