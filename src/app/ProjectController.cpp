#include "ProjectController.h"


void ProjectController::SetMesh(mesh::Mesh mesh)
{
    mesh_.emplace(std::move(mesh));
}

const mesh::Mesh &ProjectController::GetMesh() const
{
    return mesh_.value();
}