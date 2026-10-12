#pragma once
#include <string>

#include "../Mesh/Mesh2D.h"
#include "../Models/Geometry2D.h"

class VTKWriter {
  public:
    static void WriteUstructuredGrid(const Mesh &mesh,
                                     const std::string &filename) {
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

        out << "CELLS " << mesh.NumEl() << " " << mesh.NumEl() * 4 << "\n";
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
    static void saveSolution(const Mesh &mesh, Eigen::VectorXd &solution,
                             const std::string &filename) {
        if (solution.size() != mesh.NumNodes()) {
            throw std::runtime_error(
                "FEMSolver::saveSolution: solution size does "
                "not match number of mesh nodes");
        }

        std::ofstream out(filename);

        if (!out.is_open()) {
            throw std::runtime_error(
                "FEMSolver::saveSolution: cannot open file " + filename);
        }

        out << "# vtk DataFile Version 3.0\n";
        out << "FEM temperature solution\n";
        out << "ASCII\n";
        out << "DATASET UNSTRUCTURED_GRID\n";

        // Узлы
        out << "POINTS " << mesh.NumNodes() << " double\n";

        for (std::size_t nodeId = 0;
             nodeId < static_cast<std::size_t>(mesh.NumNodes()); ++nodeId) {
            const Node &node = mesh.NodeById(nodeId);

            out << node.position.x << " " << node.position.y << " " << 0.0
                << "\n";
        }

        out << "\n";

        // Треугольные элементы.
        const std::size_t numElements = static_cast<std::size_t>(mesh.NumEl());
        out << "CELLS " << numElements << " " << numElements * 4 << "\n";

        for (std::size_t elementId = 0; elementId < numElements; ++elementId) {
            const std::size_t numNodesInElement = mesh.NumNodesInEl(elementId);
            out << numNodesInElement;

            for (std::size_t localNodeId = 0; localNodeId < numNodesInElement;
                 ++localNodeId) {
                out << " " << mesh.NodeIdFromEl(elementId, localNodeId);
            }

            out << "\n";
        }

        out << "\n";

        // Для линейных треугольников VTK cell type = 5.
        out << "CELL_TYPES " << numElements << "\n";

        for (std::size_t elementId = 0; elementId < numElements; ++elementId) {
            out << "5\n";
        }

        out << "\n";

        out << "POINT_DATA " << mesh.NumNodes() << "\n";
        out << "SCALARS Temperature double 1\n";
        out << "LOOKUP_TABLE default\n";

        for (Eigen::Index nodeId = 0; nodeId < solution.size(); ++nodeId) {
            out << solution(nodeId) << "\n";
        }
    }
};
