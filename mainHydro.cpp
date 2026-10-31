#include "fem/dofs/P2DofMap.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "geometry/RectangleWithHole.h"
#include "mesh/Mesh2D.h"
#include "mesh/generators/MesherRectangle.h"
#include <cmath>
#include <numbers>

#include "physics/hydro/HydroSolver.h"
#include "physics/hydro/problem/HydroProblem.h"

#include "io/STLWriter.h"
#include "io/VTKReader.h"
#include "io/VTKWriter.h"

#include "geometry/Rectangle.h"

#include "physics/heat/HeatSolver.h"
#include "physics/heat/problem/Problem.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>


#include <cassert>
#include <iostream>

enum class ChannelHoleBoundary {
    Bottom = 1,
    Right = 2,
    Top = 3,
    Left = 4,
    Hole = 5
};

void TagChannelWithHoleBoundaries( mesh::Mesh &mesh, double L, double H,
                                  geometry::Point2D holeCenter,
                                  double holeRadius) {
    using Edge = std::pair<int, int>;

    struct PairHash {
        std::size_t operator()(const Edge &edge) const {
            auto h1 = std::hash<int>{}(edge.first);
            auto h2 = std::hash<int>{}(edge.second);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };

    std::unordered_map<Edge, int, PairHash> edgeCount;

    for (const auto &el : mesh.Elements()) {
        std::array<Edge, 3> edges = {Edge{el.nodes_id[0], el.nodes_id[1]},
                                     Edge{el.nodes_id[1], el.nodes_id[2]},
                                     Edge{el.nodes_id[2], el.nodes_id[0]}};

        for (auto [a, b] : edges) {
            if (a > b) {
                std::swap(a, b);
            }
            edgeCount[Edge{a, b}]++;
        }
    }

    constexpr double epsRect = 1e-6;
    constexpr double epsHole = 5e-4;

    for (const auto &[edge, count] : edgeCount) {
        if (count != 1) {
            continue; // не граничное ребро
        }

        const geometry::Point2D &p1 = mesh.NodeById(edge.first).position;
        const geometry::Point2D &p2 = mesh.NodeById(edge.second).position;

        const double mx = 0.5 * (p1.x + p2.x);
        const double my = 0.5 * (p1.y + p2.y);

        BoundaryId id = DefaultBoundaryId;

        if (std::abs(my - 0.0) < epsRect) {
            id = static_cast<int>(ChannelHoleBoundary::Bottom);
        }
        else if (std::abs(mx - L) < epsRect) {
            id = static_cast<int>(ChannelHoleBoundary::Right);
        }
        else if (std::abs(my - H) < epsRect) {
            id = static_cast<int>(ChannelHoleBoundary::Top);
        }
        else if (std::abs(mx - 0.0) < epsRect) {
            id = static_cast<int>(ChannelHoleBoundary::Left);
        }
        else {
            const double r = std::hypot(mx - holeCenter.x, my - holeCenter.y);

            if (std::abs(r - holeRadius) < epsHole) {
                id = static_cast<int>(ChannelHoleBoundary::Hole);
            }
            else {
                throw std::runtime_error("TagChannelWithHoleBoundaries: "
                                         "failed to classify boundary edge");
            }
        }

        mesh.AddBoundaryEdge(
            mesh::BoundaryEdge{.nodes_id = edge, .boundaryId = id, .elementId = -1});
    }

    // пересобираем boundaryEdges_ уже с корректными id и elementId
    mesh.CreateBoundaryEdges(DefaultBoundaryId);
}

int main() {

   

    // ================================================
    // Geometry and mesh
    // ================================================

    constexpr double L = 0.06;
    constexpr double H = 0.02;

    // Rectangle rectangle(H, L, Point2D{0.0, 0.0});

    //  mesh::MesherRectangle mesher(30, 15);
    //  mesh::Mesh mesh = mesher.generate mesh::Mesh(rectangle);

     mesh::Mesh mesh =
        VTKReader::ReadUstructuredGrid("channel_hole_L006_under800.vtk");

    TagChannelWithHoleBoundaries(mesh, L, H, geometry::Point2D{0.03, 0.01},
                                 0.003);

    // ================================================
    // Fluid properties
    // ================================================

    constexpr double rho = 1000.0;
    constexpr double mu = 0.01;

    constexpr double Uavg = 0.10;

    // Reynolds number based on channel height
    const double Re = rho * Uavg * H / mu;

    std::cout << "Reynolds number: " << Re << '\n';
    std::cout << "Elements: " << mesh.NumEl() << '\n';
    std::cout << "Boundary edges: " << mesh.NumBoundEdges() << '\n';

    // ================================================
    // Time parameters
    // ================================================

    constexpr double dt = 0.0025;
    constexpr double timeEnd = 0.7;

    constexpr double rampTime = 0.05;

    // ================================================
    // Inlet perturbation
    // ================================================

    constexpr double perturbationFraction = 0.00;
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

    const BoundaryId left =
        static_cast<BoundaryId>(geometry::RectangleBoundary::Left);

    const BoundaryId right =
        static_cast<BoundaryId>(geometry::RectangleBoundary::Right);

    const BoundaryId top =
        static_cast<BoundaryId>(geometry::RectangleBoundary::Top);

    const BoundaryId bottom =
        static_cast<BoundaryId>(geometry::RectangleBoundary::Bottom);

    // ================================================
    // Navier–Stokes problem
    // ================================================

    hydro::HydroProblem hydroTask(mu, rho,
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

    constexpr BoundaryId hole = 5;
    hydroTask.AddVelocityDirichlet(hole, zeroVelocity);

    // ================================================
    // Solver
    // ================================================

    hydro::HydroSolver solver(hydroTask, mesh, timeEnd, dt);

    solver.Solve("solutions/hydro/ns_channel");

    std::cout << "Navier-Stokes simulation finished.\n";

    return 0;
}