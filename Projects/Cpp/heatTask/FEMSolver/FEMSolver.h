#pragma once
#include <vector>
#include <iostream>
#include "../HeatProblem/HeatProblem.h"

class FEMSolver
{
private:
    std::vector<std::vector<double>> gMatK;
    std::vector<std::vector<double>> gMatF;
    void makeLocalMatrix();

public:
    FEMSolver(HeatProblem &hp);
    ~FEMSolver() = default;
    void makeGolbalMatrix();
    void applyBoundaryConditions();
    void solve();
    void saveSolution();
};