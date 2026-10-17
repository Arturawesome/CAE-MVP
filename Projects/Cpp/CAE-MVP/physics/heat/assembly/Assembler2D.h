#pragma once
#include <Eigen/Dense>

#include "../problem/Problem.h"
#include "../../../mesh/Mesh2D.h"
#include "../../../fem/elements/TriangleP1.h"
#include "../../../fem/systems/LinearSystem.h"

class HeatAssembler2D {
  private:
    const HeatProblem &problem_;
    const Mesh &mesh_;
    Eigen::Matrix<double, 3, 3> locK_;
    Eigen::Vector3d locF_;

  public:
    HeatAssembler2D(const HeatProblem &problem, const Mesh &mesh);
    LinearSystem Assemble();

  private:
    void MakeLocalF(const TriangleP1& triangle);
    void MakeLocalK(const TriangleP1& triangle);
    void AddToGlobal(LinearSystem& system, std::size_t elId);
    
};