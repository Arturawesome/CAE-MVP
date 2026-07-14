#pragma once
#include <functional>
#include <iostream>
#include <vector>

#include "../BoundaryCondition/BoundaryConditions.h"
#include "../Mesh/Mesh.h"

class HeatProblem {
  private:
    std::vector<BoundaryCondition> boundaryConditions;
    const Mesh &mesh;
    double lambda;

  private:
    std::function<double(double, double, double)> source;
    bool meshHasBoundaryMarker(int marker);

  public:
    HeatProblem(const Mesh &mesh_, double lambda_,
                std::function<double(double, double, double)> source_);
    ~HeatProblem() = default;

    // Getters
    const Mesh &GetMesh() const;

    double GetLambda() const;
    double GetSource(double x, double y, double z = 0.0) const;

    void AddBoundaryCondition(const BoundaryCondition &condition);
    const BoundaryCondition &GetBoundaryCondition(int marker) const;
    bool hasBoundaryCondition(int marker) const;
    int BC_size() const;
    BoundaryType GetTypeBC(int i) const;
    int GetMarkerBC(int i) const;
    double GetValue1BC(int i) const;
    double GetValue2BC(int i) const;
};