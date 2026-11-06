#include "geometry/Rectangle.h"
#include "mesh/Mesh2D.h"
#include "mesh/generators/MesherRectangle.h"
#include "physics/deformation/BoundaryCondition.h"
#include "physics/deformation/Solver.h"
#include "physics/deformation/assembly/Assembler.h"
#include "physics/deformation/problem/Problem.h"
#include <Eigen/Core>
#include <iostream>

int main() {

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

  deformation::Solver solver(problem, mesh, 2, 0.1);
  solver.Solve();

  return 0;
}