#include "FEMSolver/FEMSolver.h"
#include "Geometry/Rectangle.h"
#include "Geometry/EllipseCircle.h"

#include "HeatProblem/HeatProblem.h"
#include "Mesh/Mesh.h"
#include "Mesher/MesherRectangle.h"
#include "SetterBoundaryCondition/SetterBoundaryCondition.h"
#include <iostream>

int main() {
    // EllipseCircle circle = EllipseCircle(10, 1, 1);
    // std::cout<<circle.getHeight()<<"\n";
    // std::cout<<circle.getWidth()<<"\n";
    // std::cout<<circle.contains(5, -5)<<"\n";


    Rectangle rect = Rectangle(10, 10);
    MesherRectangle mesher = MesherRectangle(25, 25);
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