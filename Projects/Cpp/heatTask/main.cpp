
#include "Geometry/Rectangle.h"
#include "Geometry/Circle.h"
#include "HeatProblem/HeatProblem.h"
#include "Mesh/Mesh2D.h"
#include "Mesher/MesherRectangle.h"

#include "io/STLWriter.h"
#include "io/VTKWriter.h"

#include <iostream>

int main() {


    Rectangle rect = Rectangle(10, 10, Point2D{5, 5});
    Circle circle = Circle(5, Point2D{2, 2});
    MesherRectangle mesher = MesherRectangle(5, 5);
    Mesh mesh = mesher.generateMesh(rect);
    
    STLWriter::Write(mesh, "Renderer/proba1.stl");
    VTKWriter::Write(mesh, "Renderer/proba1.vtk"); 

    std::cout << "Hello CAD/CAE system\n";
    std::cout<<mesh;
    return 0;
}