#include <iostream>
#include "Geometry/Rectangle.h"
#include "Mesher/MesherRectangle.h"
#include "Mesh/Mesh.h"
#include "HeatProblem/HeatProblem.h"
#include "SetterBoundaryCondition/SetterBoundaryCondition.h"

int main()
{
    Rectangle rect = Rectangle(5, 5);
    MesherRectangle mesher = MesherRectangle(5, 3);
    Mesh mesh = mesher.generateMesh(rect);

    HeatProblem task = HeatProblem(mesh, 45, 0);
    SetterBoundaryCondition conditions = SetterBoundaryCondition(1, {1, 2},
    {100.0, 20.0});
    conditions.Apply(task);
    
    mesh.saveToSTL("mesh.stl");
    mesh.saveToVTK("mesh.vtk");
    std::cout << "Hello CAD/CAE system\n";
    return 0;
}