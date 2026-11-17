#include "ProjectController.h"

#include "io/VTKReader.h"
#include <stdexcept>

void ProjectController::LoadMesh(const std::string &filename)
{
    mesh::Mesh mesh = VTKReader::ReadUstructuredGrid(filename);
    if (mesh.NumNodes() == 0 || mesh.NumEl() == 0) {
        throw std::runtime_error("VTK mesh contains no nodes or triangles");
    }
    mesh.CreateBoundaryEdges();
    mesh_.emplace(std::move(mesh));

}
const mesh::Mesh &ProjectController::GetMesh() const
{
    return mesh_.value();
}