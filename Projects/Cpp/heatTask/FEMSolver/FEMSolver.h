#pragma once

#include "../HeatProblem/HeatProblem.h"
#include "../Mesh/Mesh2D.h"
#include <Eigen/Dense>
#include <array>
#include <iostream>
#include <string>
#include <vector>

class FEMSolver {
  private:
    const HeatProblem &problem;
    const Mesh &mesh;

    Eigen::MatrixXd triangle;
    Eigen::MatrixXd coefs;
    Eigen::MatrixXd grad;
    Eigen::MatrixXd loc_K;
    Eigen::VectorXd loc_F;

    Eigen::MatrixXd glob_K;
    Eigen::VectorXd glob_F;
    Eigen::VectorXd solution;

    double element_area = 0;
    double x_middle = 0;
    double y_middle = 0;

  private:
    void makeGlobalMatrix();
    void calcTriangleParamters(std::size_t elId);
    void makeGradMatrix(std::size_t elId);
    void makeLocalF(std::size_t elId);
    void makeLocalK(std::size_t elId);
    void addInGlobalKF(std::size_t elId);
    void addBoundCondToGlobalMat();

    void addDirichletToGlobalMatrix(int marker, double value);
    void applyDirichletToNode(int nodeId, double value);

    void addNeumannToGlobalMatrix(int marker, double flux);
    void applyNeumannToEdge(const BoundaryEdge &be, double flux);

    void addRobinToGlobalMatrix(int marker, double value1, double value2);

  public:
    FEMSolver(HeatProblem &problem_);
    ~FEMSolver() = default;
    void solve();
    void saveSolution(const std::string &filename) const;
};