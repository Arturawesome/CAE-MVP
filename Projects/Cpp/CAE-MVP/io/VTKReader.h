#pragma once
#include <string>
#include <fstream>
#include "../mesh/Mesh2D.h"


class VTKReader {
    public:
        static Mesh ReadUstructuredGrid(const std::string& filename);
    private:
        static void CheckHeaderUstructuredGrid(std::istream& in, const std::string& filename);
        static void ProcessPoints(std::istream& in, Mesh& mesh);
        static void ProcessCells(std::istream &in, std::vector<std::vector<int>>& cells);
        static void ProcessCellsTypes(std::istream &in,   std::vector<int>  &cellTypes);
};
