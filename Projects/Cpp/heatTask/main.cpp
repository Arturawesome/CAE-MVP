#include <iostream>
#include "Geometry/Rectangle.h"
#include "Mesher/MesherRectangle.h"
#include "Mesh/Mesh.h"
#include "HeatProblem/HeatProblem.h"
#include "SetterBoundaryCondition/SetterBoundaryCondition.h"
#include "FEMSolver/FEMSolver.h"

int main()
{
    Rectangle rect = Rectangle(10, 8);
    MesherRectangle mesher = MesherRectangle(25, 20);
    Mesh mesh = mesher.generateMesh(rect);

    HeatProblem task = HeatProblem(mesh, 45, [](double, double, double){
        return 10.0;
    });


    SetterBoundaryCondition conditions = SetterBoundaryCondition(1, {1, 3 },
    {100.0, 10});
    conditions.Apply(task);

    FEMSolver solver(task);
    solver.solve();
    solver.saveSolution("temperature.vtk");

    std::cout << "Hello CAD/CAE system\n";
    return 0;
}