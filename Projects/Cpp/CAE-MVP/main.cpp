#include "fem/dofs/P2DofMap.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "mesh/Mesh2D.h"
#include "mesh/generators/MesherRectangle.h"
#include <cmath>
#include <numbers>

#include "physics/hydro/HydroSolver.h"
#include "physics/hydro/problem/HydroProblem.h"

#include "io/STLWriter.h"
#include "io/VTKReader.h"
#include "io/VTKWriter.h"
#include <cassert>
#include <iostream>
int main() {

    // ================================================
    // Geometry and mesh
    // ================================================

    constexpr double L = 0.05;
    constexpr double H = 0.02;

    Rectangle rectangle(H, L, Point2D{0.0, 0.0});

    MesherRectangle mesher(30, 15);
    Mesh mesh = mesher.generateMesh(rectangle);

    // ================================================
    // Fluid properties
    // ================================================

    constexpr double rho = 1000.0;
    constexpr double mu = 0.01;

    constexpr double Uavg = 0.20;

    // Reynolds number based on channel height
    const double Re = rho * Uavg * H / mu;

    std::cout << "Reynolds number: " << Re << '\n';
    std::cout << "Elements: " << mesh.NumEl() << '\n';
    std::cout << "Boundary edges: " << mesh.NumBoundEdges() << '\n';

    // ================================================
    // Time parameters
    // ================================================

    constexpr double dt = 0.002;
    constexpr double timeEnd = 0.8;

    constexpr double rampTime = 0.05;

    // ================================================
    // Inlet perturbation
    // ================================================

    constexpr double perturbationFraction = 0.05;
    constexpr double perturbationFrequency = 20.0;

    const auto zeroVelocity = [](double, double, double) {
        return Eigen::Vector2d::Zero();
    };

    const auto inletVelocity = [](double, double y, double time) {
        const double eta = y / H;

        const double ramp = 1.0 - std::exp(-time / rampTime);

        // Parabolic Poiseuille velocity profile
        const double ux = ramp * 6.0 * Uavg * eta * (1.0 - eta);

        // Small transverse perturbation
        const double uy =
            ramp * perturbationFraction * Uavg *
            std::sin(std::numbers::pi * eta) *
            std::sin(2.0 * std::numbers::pi * perturbationFrequency * time);

        return Eigen::Vector2d(ux, uy);
    };

    // ================================================
    // Boundary markers
    // ================================================

    const BoundaryId left = static_cast<BoundaryId>(RectangleBoundary::Left);

    const BoundaryId right = static_cast<BoundaryId>(RectangleBoundary::Right);

    const BoundaryId top = static_cast<BoundaryId>(RectangleBoundary::Top);

    const BoundaryId bottom =
        static_cast<BoundaryId>(RectangleBoundary::Bottom);

    // ================================================
    // Navier–Stokes problem
    // ================================================

    HydroProblem hydroTask(mu, rho,
                           zeroVelocity, // Initial velocity
                           zeroVelocity  // Body force
    );

    // No-slip walls
    hydroTask.AddVelocityDirichlet(bottom, zeroVelocity);
    hydroTask.AddVelocityDirichlet(top, zeroVelocity);

    // Time-dependent inlet
    hydroTask.AddVelocityDirichlet(left, inletVelocity);

    // Zero traction at outlet
    hydroTask.AddTraction(right, zeroVelocity);

    // ================================================
    // Solver
    // ================================================

    HydroSolver solver(hydroTask, mesh, timeEnd, dt);

    solver.Solve("solutions/hydro/ns_channel");

    std::cout << "Navier-Stokes simulation finished.\n";

    return 0;
}