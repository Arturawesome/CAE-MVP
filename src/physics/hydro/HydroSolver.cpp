
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

  saveFrame(0, 0.0);

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
    fem::LinearSystem system =
        BuildTimeSystem(globalSystem, previousVelocity, dt_);

    // Apply boundary conditions
    HydroBoundaryAssembler2D boundayAssembler;

    boundayAssembler.Apply(system, currTime, problem_, mesh_, p2DofMap_,
                           taylorHoodDofMap_);

    // Solve linear system
    solver.compute(system.K);

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("HydroSolver: LU factorization failed");
    }

    solution_ = solver.solve(system.F);

    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("HydroSolver: linear solve failed");
    }

    if (step % 2 == 0) {
      saveFrame(step, currTime);
      std::cout << "step = " << step << "\n";
    }
  }

  seriesWriter.WriteSeries();

  const double totalTime = Duration(Clock::now() - totalStart).count();

  const double totalAssemblyTime = assemblyTime + forceTime + convectionTime;

  std::cout << "\n========== HYDRO PERFORMANCE ==========\n";

  std::cout << std::fixed << std::setprecision(3);

  std::cout << "Threads: " << numThread_ << '\n';
  std::cout << "Elements: " << mesh_.NumEl() << '\n';
  std::cout << "Time steps: " << numSteps << '\n';

  std::cout << "\nAssembly times:\n";
  std::cout << "  Initial:    " << assemblyTime << " ms\n";
  std::cout << "  Force:      " << forceTime << " ms\n";
  std::cout << "  Convection: " << convectionTime << " ms\n";

  std::cout << "\nTotal assembly: " << totalAssemblyTime << " ms\n";

  std::cout << "Assembly fraction: " << 100.0 * totalAssemblyTime / totalTime
            << " %\n";

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