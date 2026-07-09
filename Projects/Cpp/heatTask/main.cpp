#include <iostream>
#include "Geometry/Rectangle.h"
#include "Mesher/MesherRectangle.h"
#include "Mesh/Mesh.h"

int main() {
    Rectangle rect = Rectangle(5, 5);
    MesherRectangle mesher = MesherRectangle(5, 3);
    Mesh mesh = mesher.generateMesh(rect);
    mesh.saveToSTL("Renderer/mesh.stl");
    mesh.saveToVTK("Renderer/mesh.vtk");
    std::cout<<"Hello CAD/CAE system\n";
    return 0;
}