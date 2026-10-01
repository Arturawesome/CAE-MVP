#pragma once
#include <fstream>

#include "../models/Geometry2D.h"
#include "../Mesh/Mesh2D.h"

class STLWriterReader {
  public:
    static void Write(const Mesh &mesh, const std::string &filename) {
        std::ofstream out(filename);
        if (!out.is_open()) {
            throw std::runtime_error("Cannot open file " + filename);
        }
        out << "solid mesh\n";

        for (const auto &el : mesh.Elements()) {
            const Node &a = mesh.NodeById(el.nodes_id[0]);
            const Node &b = mesh.NodeById(el.nodes_id[1]);
            const Node &c = mesh.NodeById(el.nodes_id[2]);

            out << "  facet normal 0 0 1\n";
            out << "    outer loop\n";

            out << "      vertex " << a.position.x << " " << a.position.y << " 0\n";
            out << "      vertex " << b.position.x << " " << b.position.y << " 0\n";
            out << "      vertex " << c.position.x << " " << c.position.y << " 0\n";

            out << "    endloop\n";
            out << "  endfacet\n";
        }

        out << "endsolid mesh\n";
    }
};
