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

    Rectangle rect(H, L, Point2D{0.0, 0.0});
    MesherRectangle mesher(26, 10);
    Mesh rectangleMesh = mesher.generateMesh(rect);

    const BoundaryId left = static_cast<BoundaryId>(RectangleBoundary::Left);
    const BoundaryId right = static_cast<BoundaryId>(RectangleBoundary::Right);
    const BoundaryId top = static_cast<BoundaryId>(RectangleBoundary::Top);
    const BoundaryId bottom =
        static_cast<BoundaryId>(RectangleBoundary::Bottom);

    auto zeroVelocity = [](double, double, double) {
        return Eigen::Vector2d::Zero();
    };

    auto inletVelocity = [=](double, double y, double) {
        const double eta = y / H;
        return Eigen::Vector2d(6.0 * Uavg * eta * (1.0 - eta), 0.0);
    };

    auto outletTraction = [](double, double, double) {
        return Eigen::Vector2d(0.1, 0.0);
    };

    HydroProblem hydroTask(0.01, 1000.0, zeroVelocity, zeroVelocity);

    hydroTask.AddVelocityDirichlet(bottom, zeroVelocity);
    hydroTask.AddVelocityDirichlet(top, zeroVelocity);
    hydroTask.AddVelocityDirichlet(left, inletVelocity);

    hydroTask.AddTraction(right, outletTraction);

    HydroSolver solver(hydroTask, rectangleMesh, 0.5, 0.01);

    solver.Solve("solutions/hydro/traction_test");

    return 0;
}