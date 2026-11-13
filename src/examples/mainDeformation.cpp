#include "geometry/Rectangle.h"
#include "mesh/Mesh2D.h"
#include "mesh/generators/MesherRectangle.h"
#include "physics/deformation/BoundaryCondition.h"
#include "physics/deformation/Solver.h"
#include "physics/deformation/assembly/Assembler.h"
#include "physics/deformation/problem/Problem.h"
#include <Eigen/Core>
#include <iostream>
void Task1();
void Task2();

int main() {
  Task2();
  return 0;
}

void Task1() {
  constexpr double L = 0.06;
  constexpr double H = 0.02;

  geometry::Rectangle rectangle(H, L);

  //  mesh::Mesh: nx, ny are numbers of nodes
  mesh::MesherRectangle mesher(61, 21);
  mesh::Mesh mesh = mesher.generateMesh(rectangle);

  const auto forces = [](double, double, double) -> Eigen::Vector2d {
    return Eigen::Vector2d::Zero();
  };

  deformation::Problem problem(1000.0, 2e9, 0.3, forces);
  std::cout << problem.Density() << "\n";
  std::cout << problem.CoefPoisson() << "\n";
  std::cout << problem.Force(1, 1, 1) << "\n";
  std::cout << problem.ModulYoung() << "\n";
  std::cout << "CAE-MVP: Deformation module\n";

  // Left: ux = 0, uy = 0
  problem.AddDisplacementDirichlet(
      static_cast<BoundaryId>(geometry::RectangleBoundary::Left),
      [](double, double, double) -> Eigen::Vector2d {
        return Eigen::Vector2d::Zero();
      });

  // Bottom: uy = 0, ux is free
  problem.AddDisplacementY(
      static_cast<BoundaryId>(geometry::RectangleBoundary::Bottom),
      [](double, double, double) { return 0.0; });

  // Right: traction = (1 MPa, 0)
  problem.AddTraction(
      static_cast<BoundaryId>(geometry::RectangleBoundary::Right),
      [](double, double, double) -> Eigen::Vector2d { return {1.0e6, 0.0}; });

  deformation::Solver solver(problem, mesh, 2, 0.01);
  solver.Solve();
}

void Task2() {
  constexpr double L = 0.30;
  constexpr double H = 0.02;

  constexpr double rho = 1000.0;
  constexpr double E = 2.0e9;
  constexpr double nu = 0.3;

  // Total vertical force in N, assuming thickness = 1 m.
  constexpr double forceY = -10.0;
  constexpr double tractionY = forceY / H;

  geometry::Rectangle rectangle(H, L);

  // P1 elements need a reasonably fine mesh for bending.
  mesh::MesherRectangle mesher(61, 11);
  mesh::Mesh mesh = mesher.generateMesh(rectangle);

  deformation::Problem problem(rho, E, nu,
                               [](double, double, double) -> Eigen::Vector2d {
                                 return Eigen::Vector2d::Zero();
                               });

  // Fully fixed left end
  problem.AddDisplacementDirichlet(
      static_cast<BoundaryId>(geometry::RectangleBoundary::Left),
      [](double, double, double) -> Eigen::Vector2d {
        return Eigen::Vector2d::Zero();
      });

  // Downward traction on the right end
  problem.AddTraction(
      static_cast<BoundaryId>(geometry::RectangleBoundary::Right),
      [](double, double, double time) -> Eigen::Vector2d {
        constexpr double pi = 3.14159265358979323846;
        constexpr double rampTime = 0.02;

        const double s = std::min(time / rampTime, 1.0);
        const double ramp = 0.5 * (1.0 - std::cos(pi * s));

        return {0.0, tractionY * ramp};
      });

  // Top and bottom are free: no boundary conditions.

  deformation::Solver solver(problem, mesh, 0.1, 2e-4);
  solver.Solve("solutions/deformation/beam");

  std::cout << "Beam bending simulation completed.\n";
}