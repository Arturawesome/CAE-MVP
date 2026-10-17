#pragma once
#include <functional>
#include <iostream>
#include <vector>

#include "physics/heat/HeatBoundaryCondition.h"
#include "mesh/Mesh2D.h"

class HeatProblem {
  private:
    const Mesh &mesh_;
    double lambda_;
    std::vector<HeatBoundaryCondition> HeatBoundaryConditions_;

  private:
    std::function<double(double, double, double)> source_;
    
  public:
    HeatProblem(const Mesh &mesh, double lambda,
                std::function<double(double, double, double)> source);
    ~HeatProblem() = default;

    // Getters
    const Mesh &GetMesh() const;
    double Lambda() const;
    double Source(double x, double y, double z = 0.0) const;
    double Source(const Point2D& elementCenter) const;
    const std::vector<HeatBoundaryCondition>& HeatBoundaryConditions() const;

    void AddDirichlet(BoundaryId id, double temprature);
    void AddNeumann(BoundaryId id, double flux);
    void AddRobin(BoundaryId id, double alpha, double temprature);

};




















