#include "SetterBoundaryCondition.h"
#include "../HeatProblem/HeatProblem.h"

#include <iostream>
#include <stdexcept>

SetterBoundaryCondition::SetterBoundaryCondition(int type_,
                                                 const std::vector<int> &markers_,
                                                 const std::vector<double> &values_)
    : type(static_cast<BoundaryType>(type_)), markers(markers_), values(values_)
{
    std::cout << "SetterBoundaryCondition::SetterBoundaryCondition\n";
    if (markers.empty())
    {
        throw std::invalid_argument(
            "SetterBoundaryCondition: markers cannot be empty");
    }

    if (markers.size() != values.size())
    {
        throw std::invalid_argument(
            "SetterBoundaryCondition: markers and values must have equal sizes");
    }
}

void SetterBoundaryCondition::Apply(HeatProblem &problem) const
{
    switch (type)
    {
    case BoundaryType::Dirichlet:
        ApplyDirichlet(problem);
        break;
    case BoundaryType::Neumann:
        throw std::invalid_argument("SetterBoundaryCondition: Neumann condition is not implemented");
        break;
    case BoundaryType::Robin:
        throw std::invalid_argument("SetterBoundaryCondition: Robin condition is not implemented");
        break;
    default:
        throw std::invalid_argument("SetterBoundaryCondition: unknown boundary condition type");
        break;
    }
}

void SetterBoundaryCondition::ApplyDirichlet(HeatProblem &problem) const
{
    for (std::size_t i = 0; i < markers.size(); ++i)
    {
        problem.AddBoundaryCondition({
            //.marker = 
            markers[i],
            //.type = 
            BoundaryType::Dirichlet,
            //.value1 = 
            values[i],
            //.value2 = 
            0.0
        });
    }
}