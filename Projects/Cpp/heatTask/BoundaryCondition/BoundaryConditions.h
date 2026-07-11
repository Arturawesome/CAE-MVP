#pragma once

// TO DO special class for Baoundaru condiions setting

enum class BoundaryType
{
    Dirichlet,
    Neumann,
    Robin
};



struct BoundaryCondition
{
    int marker = 0;
    BoundaryType type = BoundaryType::Dirichlet;

    double value1 = 0;
    double value2 = 0;
};
