#include "FEMSolver/FEMSolver.h"
#include "Geometry/Rectangle.h"
#include "HeatProblem/HeatProblem.h"
#include "Mesh/Mesh.h"
#include "Mesher/MesherRectangle.h"
#include "SetterBoundaryCondition/SetterBoundaryCondition.h"
#include <iostream>

int main() {
    Rectangle rect = Rectangle(10, 8);
    MesherRectangle mesher = MesherRectangle(25, 20);
    Mesh mesh = mesher.generateMesh(rect);

    HeatProblem task =
        HeatProblem(mesh, 45, [](double, double, double) { return 10.0; });

    SetterBoundaryCondition conditions = SetterBoundaryCondition(2, {1}, {10});
    conditions.Apply(task);

    SetterBoundaryCondition conditions2 = SetterBoundaryCondition(1, {2}, {30});
    conditions2.Apply(task);

    FEMSolver solver(task);
    solver.solve();
    solver.saveSolution("temperature.vtk");

    std::cout << "Hello CAD/CAE system\n";
    return 0;
}