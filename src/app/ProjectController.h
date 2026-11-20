#pragma once
#include "mesh/Mesh2D.h"
#include <optional>
#include <string>


class ProjectController
{
public:
    void SetMesh(mesh::Mesh mesh);
    const mesh::Mesh &GetMesh() const;
private:
    std::optional<mesh::Mesh> mesh_;
};
