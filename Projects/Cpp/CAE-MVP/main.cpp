#include "Geometry/Circle.h"
#include "Geometry/Rectangle.h"
#include "HeatProblem/HeatProblem.h"
#include "Mesh/Mesh2D.h"
#include "Mesher/MesherRectangle.h"
#include "fem/FEMSolver.h"

#include "io/STLWriter.h"
#include "io/VTKReader.h"
#include "io/VTKWriter.h"
#include <iostream>
#include <cassert>
int main() {
    Rectangle rect = Rectangle(10, 10, Point2D{5, 5});
    Circle circle = Circle(5, Point2D{2, 2});
    MesherRectangle mesher = MesherRectangle(5, 5);
    Mesh mesh = mesher.generateMesh(rect);

    STLWriter::Write(mesh, "Renderer/proba1.stl");
    VTKWriter::WriteUstructuredGrid(mesh, "Renderer/proba1.vtk");

    Mesh circleMesh =
        VTKReader::ReadUstructuredGrid("circle_2d_unstructured.vtk");
    circleMesh.CreateBoundaryEdges();

    std::cout << circleMesh.NumNodes() << '\n';
    std::cout << circleMesh.NumEl() << '\n';
    std::cout << circleMesh.NumBoundEdges() << "\n";

    HeatProblem task =
        HeatProblem(circleMesh, 45, [](double, double, double) { return 500.0; });
    task.AddRobin(1, 10, 50);


    FEMSolver solver(task);
    solver.Solve();
    VTKWriter::saveSolution(circleMesh, solver.Solution(), "ioCircleSolutionRefactor.vtk");


   
    return 0;
}