#include "fem/dofs/P2DofMap.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "mesh/Mesh2D.h"
#include "mesh/generators/MesherRectangle.h"
#include <cmath>
#include <numbers>
// #include "physics/heat/HeatSolver.h"
// #include "physics/heat/problem/Problem.h"

#include "physics/hydro/HydroSolver.h"
#include "physics/hydro/problem/HydroProblem.h"

#include "io/STLWriter.h"
#include "io/VTKReader.h"
#include "io/VTKWriter.h"
#include <cassert>
#include <iostream>
int main() {
    constexpr double H = 0.02;
    constexpr double L = 0.1;
    constexpr double Uavg = 0.02;

    Rectangle rect = Rectangle(H, L, Point2D{0, 0});

    MesherRectangle mesher = MesherRectangle(26, 10);
    Mesh rectangleMesh = mesher.generateMesh(rect);

    // Mesh circleMesh =
    //     VTKReader::ReadUstructuredGrid("circle_2d_unstructured.vtk");
    // circleMesh.CreateBoundaryEdges();

    P2DofMap p2Dof(rectangleMesh);
    for (const auto el : p2Dof.NodesIdByElementId(4)) {
        std::cout << el << " ";
    }

    // HeatProblem task = HeatProblem(
    //     rectangleMesh, 45, [](double, double, double) { return 500.0; });
    // task.AddRobin(1, 10, 50);
    // HeatSolver heatSolver(task);
    // heatSolver.Solve();

    auto zeroVelocity = [](double, double, double) -> Eigen::Vector2d {
        return Eigen::Vector2d::Zero();
    };

    constexpr double U0 = 0.02;
    constexpr double frequency = 2.0;

    // auto inletVelocity = [=](double x, double y, double t) {
    //     const double amplitude =
    //         U0 * (1.0 + 0.5 * std::sin(2.0 * std::numbers::pi * frequency *
    //         t));

    //     const double u = 6.0 * amplitude * (y / H) * (1.0 - y / H);

    //     return Eigen::Vector2d(u, 0.0);
    // };
    // auto inletVelocity = [=](double x, double y, double t) {
    //     const double tau = 0.1;
    //     const double U = U0 * (1.0 - std::exp(-t / tau));
    //     const double ux = 6.0 * U * (y / H) * (1.0 - y / H);
    //     return Eigen::Vector2d(ux, 0.0);
    // };

    
    constexpr double amplitude = 0.6;
    

    auto inletVelocity = [=](double x, double y, double time) {
        const double U =
            U0 * (1.0 + amplitude * std::sin(2.0 * std::numbers::pi *
                                             frequency * time));

        const double ux = 6.0 * U * (y / H) * (1.0 - y / H);

        return Eigen::Vector2d(ux, 0.0);
    };

    // auto initialVelocity = [=](double, double y, double) -> Eigen::Vector2d {
    //     const double eta = y / H;
    //     return {6.0 * Uavg * eta * (1.0 - eta), 0.0};
    // };

    auto initialVelocity = [](double x, double y, double t) {
        return Eigen::Vector2d(0.0, 0.0);
    };

    auto externalForce = [](double, double, double) -> Eigen::Vector2d {
        return Eigen::Vector2d::Zero();
    };

    HydroProblem hydroTask(0.01, 1000.0, externalForce, initialVelocity);

    hydroTask.AddVelocityDirichlet(
        static_cast<BoundaryId>(RectangleBoundary::Bottom), zeroVelocity);

    hydroTask.AddVelocityDirichlet(
        static_cast<BoundaryId>(RectangleBoundary::Top), zeroVelocity);

    hydroTask.AddVelocityDirichlet(
        static_cast<BoundaryId>(RectangleBoundary::Left), inletVelocity);

    hydroTask.AddTraction(static_cast<BoundaryId>(RectangleBoundary::Right),
                          zeroVelocity);

    HydroSolver hydroSolver(hydroTask, rectangleMesh, 1.0, 0.01);

    hydroSolver.Solve();
    return 0;
}