#pragma once
#include "P2DofMap.h"
#include "mesh/Mesh2D.h"

#pragma once

#include <array>

#include "fem/dofs/P2DofMap.h"
#include "mesh/Mesh2D.h"
namespace fem {
class TaylorHoodDofMap {
  private:
    const mesh::Mesh &mesh_;
    const P2DofMap &p2_;

  public:
    TaylorHoodDofMap(const mesh::Mesh &mesh, const P2DofMap &p2)
        : mesh_(mesh), p2_(p2) {}

    // two vel components for X and Y and + size for pressure
    int NumDofs() const { return 2 * p2_.NumP2Nodes() + mesh_.NumNodes(); }
    int numVelocityDofs() const { return 2 * p2_.NumP2Nodes(); }
    int numPressureDofs() const { return mesh_.NumNodes(); }

    int U(int nodeId) const { return nodeId; }

    int V(int nodeId) const { return p2_.NumP2Nodes() + nodeId; }

    int P(int nodeId) const { return 2 * p2_.NumP2Nodes() + nodeId; }
    int MiddleNodeId(int nodeId1, int nodeId2) const {
        return p2_.MiddleNodeId(nodeId1, nodeId2);
    }

    std::array<int, 15> ElementDofs(int elId) const {
        std::array<int, 15> dofs{};

        const auto &velocityNodes = p2_.NodesIdByElementId(elId);

        const auto &pressureNodes = mesh_.ElementById(elId).nodes_id;

        for (int i = 0; i < 6; ++i) {
            dofs[2 * i] = U(velocityNodes[i]);

            dofs[2 * i + 1] = V(velocityNodes[i]);
        }

        for (int i = 0; i < 3; ++i) {
            dofs[12 + i] = P(pressureNodes[i]);
        }
        return dofs;
    }
};
} // namespace fem