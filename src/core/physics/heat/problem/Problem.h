#pragma once
#include <functional>
#include <iostream>
#include <vector>

#include "mesh/Mesh2D.h"
#include "physics/heat/HeatBoundaryCondition.h"
namespace heat {
class HeatProblem {
  private:
    const mesh::Mesh &mesh_;
    double lambda_;
    std::vector<HeatBoundaryCondition> HeatBoundaryConditions_;

  private:
    std::function<double(double, double, double)> source_;

  public:
    HeatProblem(const mesh::Mesh &mesh, double lambda,
                std::function<double(double, double, double)> source);
    ~HeatProblem() = default;

    // Getters
    const mesh::Mesh &GetMesh() const;
    double Lambda() const;
    double Source(double x, double y, double z = 0.0) const;
    double Source(const geometry::Point2D &elementCenter) const;
    const std::vector<HeatBoundaryCondition> &HeatBoundaryConditions() const;

    void AddDirichlet(BoundaryId id, double temprature);
    void AddNeumann(BoundaryId id, double flux);
    void AddRobin(BoundaryId id, double alpha, double temprature);
};

} // namespace heat
