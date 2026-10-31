#pragma once

#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/heat/problem/Problem.h"

#include <unordered_map>

class HeatBoundaryAssembler2D {
private:
    std::unordered_map<int, double> CollectDirichlet(const HeatProblem& problem,
                                                     const Mesh& mesh) const;
    void AddNeumann(LinearSystem& system, const Mesh& mesh,
                    BoundaryId boundaryId, double flux) const;
    void AddRobin(LinearSystem& system, const Mesh& mesh,
                  BoundaryId boundaryId, double alpha, double temperature) const;
    void ApplyDirichlet(LinearSystem& system,
                        const std::unordered_map<int, double>& fixedNodes) const;

public:
    void Apply(LinearSystem& system, const HeatProblem& problem,
               const Mesh& mesh) const;
};
