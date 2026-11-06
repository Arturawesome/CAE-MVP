#pragma once
#include <string>
#include <fstream>
#include "mesh/Mesh2D.h"

#include <cmath>
#include <unordered_map>
#include <utility>
#include <vector>
#include <stdexcept>


class VTKReader {
    public:
        static mesh::Mesh ReadUstructuredGrid(const std::string& filename);
    private:
        static void CheckHeaderUstructuredGrid(std::istream& in, const std::string& filename);
        static void ProcessPoints(std::istream& in, mesh::Mesh& mesh);
        static void ProcessCells(std::istream &in, std::vector<std::vector<int>>& cells);
        static void ProcessCellsTypes(std::istream &in,   std::vector<int>  &cellTypes);
};
