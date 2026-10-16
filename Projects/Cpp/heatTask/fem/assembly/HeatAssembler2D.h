#pragma once
#include <Eigen/Dense>

#include "../../HeatProblem/HeatProblem.h"
#include "../../Mesh/Mesh2D.h"
#include "../systems/Linearsystem.h"
#include "../elements/TriangleP1.h"

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
    void AddToGlobal(std::size_t elId);
    void AddToGlobal(LinearSystem& system, Eigen::Vector3d& locF,  Eigen::Matrix<double, 3, 3>& locK);
};