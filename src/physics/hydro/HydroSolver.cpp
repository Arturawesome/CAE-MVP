
#include "HydroSolver.h"

#include "io/VTKHydroWriter.h"
#include "io/VTKSeriesWriter.h"
#include "physics/hydro/assembly/HydroAssembler.h"
#include "physics/hydro/assembly/HydroBoundaryAssembler.h"
#include <Eigen/SparseLU>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace hydro {
HydroSolver::HydroSolver(const HydroProblem &problem, mesh::Mesh &mesh,
                         double timeEnd, double dt, size_t numThread)
    : problem_(problem), mesh_(mesh), timeEnd_(timeEnd), dt_(dt),
      numThread_(numThread), p2DofMap_(mesh_),
      taylorHoodDofMap_(mesh_, p2DofMap_) {

  solution_ = Eigen::VectorXd::Zero(taylorHoodDofMap_.NumDofs());
  if (dt_ <= 0.0 || timeEnd_ <= 0.0) {
    throw std::invalid_argument("HydroSolver: invalid time parameters");
  }
  if (numThread_ == 0.0) {
    throw std::invalid_argument("HydroSolver: invalid numThread parameters");
  }
}

void HydroSolver::Solve(std::string filename) {
  using Clock = std::chrono::steady_clock;
  using Duration = std::chrono::duration<double, std::milli>;

  const auto totalStart = Clock::now();

  double assemblyTime = 0.0;
  double forceTime = 0.0;
  double convectionTime = 0.0;

  double buildTime = 0.0;
  double boundaryTime = 0.0;
  double factorizationTime = 0.0;
  double linearSolveTime = 0.0;
  double vtkTime = 0.0;

  HydroAssembler2D assembler(problem_, mesh_, taylorHoodDofMap_, numThread_);

  // Initial assembly
  auto start = Clock::now();

  auto globalSystem = assembler.AssembleGlobalSystemPar();

  assemblyTime = Duration(Clock::now() - start).count();

  const int numSteps = static_cast<int>(std::ceil(timeEnd_ / dt_));

  auto currVelocity = MakeInitialVelocity();

  solution_.head(currVelocity.size()) = currVelocity;

  const std::filesystem::path outputDir = filename;
  std::filesystem::create_directories(outputDir);

  VTKSeriesWriter seriesWriter(outputDir);

  const auto saveFrame = [&](int step, double time) {
    std::ostringstream stream;

    stream << "hydro_" << std::setw(5) << std::setfill('0') << step << ".vtk";

    const std::string filename = stream.str();

    VTKHydroWriter::Write(mesh_, taylorHoodDofMap_, solution_,
                          (outputDir / filename).string());

    seriesWriter.AddFrame(filename, time);
  };

  start = Clock::now();

  saveFrame(0, 0.0);

  vtkTime += Duration(Clock::now() - start).count();

  Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

  for (int step = 1; step <= numSteps; ++step) {

    double currTime = step * dt_;

    const Eigen::VectorXd previousVelocity =
        solution_.head(globalSystem.mass.rows());

    // Update force
    start = Clock::now();

    assembler.UpdateForcePar(globalSystem, currTime);

    forceTime += Duration(Clock::now() - start).count();

    // Update convection
    start = Clock::now();

    assembler.UpdateConvectionPar(globalSystem, previousVelocity);

    convectionTime += Duration(Clock::now() - start).count();

    // Build time system
    start = Clock::now();

    fem::LinearSystem system =
        BuildTimeSystem(globalSystem, previousVelocity, dt_);

    buildTime += Duration(Clock::now() - start).count();

    // Apply boundary conditions
    start = Clock::now();

    HydroBoundaryAssembler2D boundaryAssembler;

    boundaryAssembler.Apply(system, currTime, problem_, mesh_, p2DofMap_,
                            taylorHoodDofMap_);

    boundaryTime += Duration(Clock::now() - start).count();

    // LU factorization
    start = Clock::now();

    solver.compute(system.K);

    factorizationTime += Duration(Clock::now() - start).count();

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("HydroSolver: LU factorization failed");
    }

    // Linear solve
    start = Clock::now();

    solution_ = solver.solve(system.F);

    linearSolveTime += Duration(Clock::now() - start).count();

    if (solver.info() != Eigen::Success || !solution_.allFinite()) {
      throw std::runtime_error("HydroSolver: linear solve failed");
    }

    // VTK output
    if (step % 2 == 0) {
      start = Clock::now();

      saveFrame(step, currTime);

      vtkTime += Duration(Clock::now() - start).count();

      std::cout << "step = " << step << '\n';
    }
  }

  seriesWriter.WriteSeries();

  const double totalTime = Duration(Clock::now() - totalStart).count();

  const double totalAssemblyTime = assemblyTime + forceTime + convectionTime;

  const double otherTime =
      totalTime -
      (assemblyTime + forceTime + convectionTime + buildTime + boundaryTime +
       factorizationTime + linearSolveTime + vtkTime);

  std::cout << "\n========== HYDRO PERFORMANCE ==========\n";

  std::cout << std::fixed << std::setprecision(3);

  std::cout << "Threads: " << numThread_ << '\n';
  std::cout << "Elements: " << mesh_.NumEl() << '\n';
  std::cout << "Time steps: " << numSteps << '\n';

  std::cout << "\nAssembly:\n";
  std::cout << "  Initial:      " << assemblyTime << " ms\n";
  std::cout << "  Force:        " << forceTime << " ms\n";
  std::cout << "  Convection:   " << convectionTime << " ms\n";

  std::cout << "\nOther operations:\n";
  std::cout << "  Build system: " << buildTime << " ms\n";
  std::cout << "  Boundary:     " << boundaryTime << " ms\n";
  std::cout << "  LU compute:   " << factorizationTime << " ms\n";
  std::cout << "  LU solve:     " << linearSolveTime << " ms\n";
  std::cout << "  VTK output:   " << vtkTime << " ms\n";
  std::cout << "  Unmeasured:   " << otherTime << " ms\n";

  std::cout << "\nTOTAL SOLVE TIME: " << totalTime << " ms\n";

  std::cout << "=======================================\n";
}

Eigen::VectorXd HydroSolver::MakeInitialVelocity() const {
  const int numNodes = p2DofMap_.NumP2Nodes();
  Eigen::VectorXd velocity = Eigen::VectorXd::Zero(2 * numNodes);
  for (int nodeId = 0; nodeId < numNodes; nodeId++) {
    auto point = p2DofMap_.P2NodeById(nodeId);

    auto nodeVelxy =
        problem_.InitialVelocity(point.position.x, point.position.y);
    velocity(taylorHoodDofMap_.U(nodeId)) = nodeVelxy.x();

    velocity(taylorHoodDofMap_.V(nodeId)) = nodeVelxy.y();
  }
  return velocity;
}

fem::LinearSystem
HydroSolver::BuildTimeSystem(const HydroGlobalSystem &global,
                             const Eigen::VectorXd &previousVelocity,
                             double dt) const {
  using SparseMatrix = Eigen::SparseMatrix<double>;
  using Triplet = Eigen::Triplet<double>;
  if (dt <= 0.0) {
    throw std::invalid_argument("HydroSolver: time step must be positive");
  }

  int numVelocityDofs = global.mass.rows();
  int numPressureDofs = global.divergence.rows();
  int numDofs = numVelocityDofs + numPressureDofs;
  if (global.mass.cols() != numVelocityDofs ||
      global.viscosity.rows() != numVelocityDofs ||
      global.viscosity.cols() != numVelocityDofs ||
      global.divergence.cols() != numVelocityDofs ||
      global.force.size() != numVelocityDofs ||
      previousVelocity.size() != numVelocityDofs) {

    throw std::invalid_argument("HydroSolver: incompatible matrix dimensions");
  }

  fem::LinearSystem system(numDofs);
  std::vector<Triplet> triplets;
  triplets.reserve(global.mass.nonZeros() + global.viscosity.nonZeros() +
                   2 * global.divergence.nonZeros() +
                   global.convection.nonZeros());

  // 1. Mass matrix: M / dt
  // outerSize -- size dense matrix
  for (int k = 0; k < global.mass.outerSize(); ++k) {
    // throuth the all not zero elements
    for (SparseMatrix::InnerIterator it(global.mass, k); it; ++it) {
      triplets.emplace_back(it.row(), it.col(), it.value() / dt);
    }
  }

  // 2. Viscosity matrix: A
  for (int k = 0; k < global.viscosity.outerSize(); ++k) {
    for (SparseMatrix::InnerIterator it(global.viscosity, k); it; ++it) {
      triplets.emplace_back(it.row(), it.col(), it.value());
    }
  }
  // 3. Pressure-velocity coupling
  for (int k = 0; k < global.divergence.outerSize(); ++k) {
    for (SparseMatrix::InnerIterator it(global.divergence, k); it; ++it) {
      int pressureID = numVelocityDofs + it.row();
      int velocityID = it.col();
      triplets.emplace_back(pressureID, velocityID, it.value());
      triplets.emplace_back(velocityID, pressureID, -it.value());
    }
  }

  // 4. Convection matrix C(U^n)
  for (int k = 0; k < global.convection.outerSize(); ++k) {
    for (Eigen::SparseMatrix<double>::InnerIterator it(global.convection, k);
         it; ++it) {
      triplets.emplace_back(it.row(), it.col(), it.value());
    }
  }
  system.K.setFromTriplets(triplets.begin(), triplets.end());
  system.K.makeCompressed();
  system.F.head(numVelocityDofs) =
      global.force + (global.mass * previousVelocity) / dt;
  return system;
}

const Eigen::VectorXd &HydroSolver::Solution() { return solution_; }
} // namespace hydro