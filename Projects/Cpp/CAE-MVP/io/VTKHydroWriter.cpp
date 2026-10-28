
#include "VTKHydroWriter.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

void VTKHydroWriter::Write(
    const Mesh& mesh,
    const TaylorHoodDofMap& dofMap,
    const Eigen::VectorXd& solution,
    const std::string& filename
) {

    if (solution.size() != dofMap.NumDofs()) {
        throw std::invalid_argument(
            "VTKHydroWriter: solution size mismatch"
        );
    }

    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error(
            "VTKHydroWriter: cannot open file " + filename
        );
    }

    out << std::setprecision(17);

    // VTK header
    out << "# vtk DataFile Version 3.0\n";
    out << "Hydrodynamics FEM solution\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";

    // 1. Mesh nodes
    out << "POINTS " << mesh.NumNodes() << " double\n";

    for (const auto& node : mesh.Nodes()) {
        out << node.position.x << " "
            << node.position.y << " 0.0\n";
    }

    out << "\n";

    // 2. Mesh elements
    const int numElements = mesh.NumEl();

    out << "CELLS " << numElements
        << " " << numElements * 4 << "\n";

    for (const auto& element : mesh.Elements()) {
        out << "3 "
            << element.nodes_id[0] << " "
            << element.nodes_id[1] << " "
            << element.nodes_id[2] << "\n";
    }

    out << "\n";

    // 3. VTK triangle type = 5
    out << "CELL_TYPES " << numElements << "\n";

    for (int i = 0; i < numElements; ++i) {
        out << "5\n";
    }

    out << "\n";

    // 4. Nodal fields
    out << "POINT_DATA " << mesh.NumNodes() << "\n";

    // Velocity vector
    out << "VECTORS Velocity double\n";

    for (int nodeId = 0; nodeId < mesh.NumNodes(); ++nodeId) {
        const double u =
            solution(dofMap.U(nodeId));

        const double v =
            solution(dofMap.V(nodeId));

        out << u << " " << v << " 0.0\n";
    }

    out << "\n";

    // Velocity magnitude
    out << "SCALARS VelocityMagnitude double 1\n";
    out << "LOOKUP_TABLE default\n";

    for (int nodeId = 0; nodeId < mesh.NumNodes(); ++nodeId) {
        const double u =
            solution(dofMap.U(nodeId));

        const double v =
            solution(dofMap.V(nodeId));

        out << std::hypot(u, v) << "\n";
    }

    out << "\n";

    // Pressure
    out << "SCALARS Pressure double 1\n";
    out << "LOOKUP_TABLE default\n";

    for (int nodeId = 0; nodeId < mesh.NumNodes(); ++nodeId) {
        out << solution(dofMap.P(nodeId)) << "\n";
    }

    if (!out) {
        throw std::runtime_error(
            "VTKHydroWriter: write failed"
        );
    }
}
