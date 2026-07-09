#include "Mesh.h"

#include <fstream>
#include <stdexcept>

void Mesh::saveToVTK(const std::string& filename) const {
    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }

    out << "# vtk DataFile Version 3.0\n";
    out << "Triangle mesh\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";

    out << "POINTS " << nodes.size() << " double\n";
    for (const auto& node : nodes) {
        out << node.x << " " << node.y << " 0.0\n";
    }

    out << "\n";

    out << "CELLS " << elements.size() << " " << elements.size() * 4 << "\n";
    for (const auto& el : elements) {
        out << "3 "
            << el.nodes_id[0] << " "
            << el.nodes_id[1] << " "
            << el.nodes_id[2] << "\n";
    }

    out << "\n";

    out << "CELL_TYPES " << elements.size() << "\n";
    for (size_t i = 0; i < elements.size(); ++i) {
        out << "5\n";
    }
}

void Mesh::saveToSTL(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }
    out << "solid mesh\n";

    for (const auto& el : elements) {
        const Node& a = nodes[el.nodes_id[0]];
        const Node& b = nodes[el.nodes_id[1]];
        const Node& c = nodes[el.nodes_id[2]];

        out << "  facet normal 0 0 1\n";
        out << "    outer loop\n";

        out << "      vertex " << a.x << " " << a.y << " 0\n";
        out << "      vertex " << b.x << " " << b.y << " 0\n";
        out << "      vertex " << c.x << " " << c.y << " 0\n";

        out << "    endloop\n";
        out << "  endfacet\n";
    }

    out << "endsolid mesh\n";
}