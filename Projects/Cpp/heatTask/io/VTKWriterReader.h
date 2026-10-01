#pragma once
#include <string>

#include "../models/Geometry2D.h"
#include "../Mesh/Mesh2D.h"

class VTKWriterReader {
  public:
    static void Write(const Mesh &mesh, const std::string &filename)  {
        std::ofstream out(filename);

        if (!out.is_open()) {
            throw std::runtime_error("Cannot open file " + filename);
        }

        out << "# vtk DataFile Version 3.0\n";
        out << "Triangle mesh\n";
        out << "ASCII\n";
        out << "DATASET UNSTRUCTURED_GRID\n";

        out << "POINTS " << mesh.NumNodes() << " double\n";
        for (const auto &node : mesh.Nodes()) {
            out << node.position.x << " " << node.position.y << " 0.0\n";
        }

        out << "\n";

        out << "CELLS " << mesh.NumEl() << " " << mesh.NumEl() * 4
            << "\n";
        for (const auto &el : mesh.Elements()) {
            out << "3 " << el.nodes_id[0] << " " << el.nodes_id[1] << " "
                << el.nodes_id[2] << "\n";
        }

        out << "\n";

        out << "CELL_TYPES " << mesh.NumEl() << "\n";
        for (int i = 0; i < mesh.NumEl(); ++i) {
            out << "5\n";
        }
    }
};
