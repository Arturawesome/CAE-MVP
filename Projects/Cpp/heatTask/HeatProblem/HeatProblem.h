#pragma once
#include <iostream>
#include <vector>
#include "../Mesh/Mesh.h"
#include "../BoundaryCondition/BoundaryConditions.h"

class HeatProblem
{
private:
    const Mesh &mesh;

    double lambda;
    double source;

    std::vector<BoundaryCondition> boundaryConditions;

private:
    bool meshHasBoundaryMarker(int marker);

public:
    HeatProblem(const Mesh &mesh_, double lambda_, double source_);
    ~HeatProblem() = default;

    // Getters
    double GetLambda() const;
    double GetSource() const;
    const Mesh &GetMesh() const;

    void AddBoundaryCondition(const BoundaryCondition &condition);
    const BoundaryCondition &GetBoundaryCondition(int marker) const;
    bool hasBoundaryCondition(int marker) const;
};