#include "io/VTKWriter.h"
#include <filesystem>

#include "geometry/Rectangle.h"
#include "mesh/generators/MesherRectangle.h"
#include "physics/heat/HeatSolver.h"
#include "physics/heat/problem/Problem.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

int main() {
    try {
        // Geometry
        constexpr double L = 0.06;
        constexpr double H = 0.02;

        geometry::Rectangle rectangle(H, L);

        //  mesh::Mesh: nx, ny are numbers of nodes
         mesh::MesherRectangle mesher(61, 21);
         mesh::Mesh mesh = mesher.generateMesh(rectangle);

        // Heat problem
        constexpr double lambda = 10.0;

        heat::HeatProblem problem(mesh, lambda, [](double, double, double) {
            return 0.0; // No heat source
        });

        // Boundary conditions
        problem.AddDirichlet(static_cast<BoundaryId>(geometry::RectangleBoundary::Left),
                             100.0);

        problem.AddDirichlet(static_cast<BoundaryId>(geometry::RectangleBoundary::Right),
                             200.0);

        problem.AddNeumann(static_cast<BoundaryId>(geometry::RectangleBoundary::Top),
                           0.0);

        problem.AddNeumann(static_cast<BoundaryId>(geometry::RectangleBoundary::Bottom),
                           0.0);

        // Solve
        heat::HeatSolver solver(problem);
        solver.Solve();

        const auto &solution = solver.Solution();

        // Compare against analytical solution
        double maxError = 0.0;

        for (int i = 0; i < mesh.NumNodes(); ++i) {
            const auto &point = mesh.NodeById(i).position;

            const double expected = 100.0 + 100.0 * point.x / L;
            const double error = std::abs(solution(i) - expected);

            maxError = std::max(maxError, error);
        }

        std::cout << "Nodes: " << mesh.NumNodes() << '\n';
        std::cout << "Elements: " << mesh.NumEl() << '\n';
        std::cout << "Min T: " << solution.minCoeff() << '\n';
        std::cout << "Max T: " << solution.maxCoeff() << '\n';
        std::cout << "Max error: " << maxError << '\n';

        constexpr double tolerance = 1e-8;

        if (maxError > tolerance) {
            throw std::runtime_error(
                "Heat test failed: analytical solution mismatch");
        }

        std::cout << "Heat test PASSED\n";

        // Save temperature field for ParaView
        std::filesystem::create_directories("solutions/heat");

        VTKWriter::saveSolution(mesh, solver.Solution(),
                                "solutions/heat/temperature.vtk");

        std::cout << "VTK saved: solutions/heat/temperature.vtk\n";
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    
    return 0;
}
