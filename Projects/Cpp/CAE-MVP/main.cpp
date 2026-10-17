#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "mesh/Mesh2D.h"
#include "mesh/generators/MesherRectangle.h"

#include "physics/heat/problem/Problem.h"
#include "physics/heat/HeatSolver.h"

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


    Mesh circleMesh =
        VTKReader::ReadUstructuredGrid("circle_2d_unstructured.vtk");
    circleMesh.CreateBoundaryEdges();

    std::cout << circleMesh.NumNodes() << '\n';
    std::cout << circleMesh.NumEl() << '\n';
    std::cout << circleMesh.NumBoundEdges() << "\n";

    HeatProblem task =
        HeatProblem(circleMesh, 45, [](double, double, double) { return 500.0; });
    task.AddRobin(1, 10, 50);


    HeatSolver solver(task);
    solver.Solve();
    VTKWriter::saveSolution(circleMesh, solver.Solution(), "solutions/CircleHeat.vtk");


   
    return 0;
}