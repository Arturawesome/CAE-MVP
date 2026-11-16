#pragma once
#include "mesh/Mesh2D.h"
#include <optional>
#include <string>


class ProjectController
{
public:
    void LoadMesh(const std::string &filename);
    const mesh::Mesh &GetMesh() const;
private:
    std::optional<mesh::Mesh> mesh_;
};
