#pragma once
#include <vector>
#include <iostream>
#include <array>

#include "../HeatProblem/HeatProblem.h"

class FEMSolver
{
private:
    //matr 3х3 ((1,1,1),(xi,xj,xk),(yi,yj,yk))
    std::array<std::array<int, 3>, 3> triangle;
    // матрица 3х3 коэффициентов a,b,c ((ai,aj,ak),(bi,bj,bk),(ci,cj,ck))
    std::array<std::array<int, 3>, 3> coefs;
    // матрица 2х3 градиентов ((bi,bj,bk),(ci,cj,ck))
    std::array<std::array<int, 2>, 3> grad;
    // локальная матрица жесткости  
    std::array<std::array<int, 3>, 3> loc_K;
    // локальный вектор правых частей 
    std::array<int, 3> loc_F;

    // global matrix
    std::vector<std::vector<double>> glob_K;
    std::vector<std::vector<double>> glob_F;
    void makeLocalMatrix();

public:
    FEMSolver(HeatProblem &hp);
    ~FEMSolver() = default;
    void makeGolbalMatrix();
    void applyBoundaryConditions();
    void solve();
    void saveSolution();
};