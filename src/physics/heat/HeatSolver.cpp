#include "HeatSolver.h"

#include "LogDuration.h"
#include "assembly/Assembler2D.h"
#include "assembly/BoundaryAssembler2D.h"

#include <Eigen/SparseCholesky>
#include <fstream>
#include <stdexcept>

namespace heat {
HeatSolver::HeatSolver(HeatProblem &problem, unsigned int threadCount,
                       EigenMethod method)
    : problem_(problem), mesh_(problem.GetMesh()), threadCount_(threadCount),
      method_(method) {

  std::cout << "HeatSolver::HeatSolver\n";
  int numNodes = mesh_.NumNodes();
  solution_ = Eigen::VectorXd::Zero(numNodes);
}

void HeatSolver::Solve() {
  std::cout << "HeatSolver: solve\n";
  fem::LinearSystem system;
  {
    LogDuration timer("assembler");
    HeatAssembler2D assembler(problem_, mesh_, threadCount_);
    system = assembler.Assemble();
  }

  {
    LogDuration timer("assembler boundary");
    HeatBoundaryAssembler2D boundaryAssembler;
    boundaryAssembler.Apply(system, problem_, mesh_);
  }
  if (method_ == EigenMethod::SPARSE_LU) {
    LogDuration timer("solve: SparseLU");
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

    solver.compute(system.K);
    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("HeatSolver: matrix factorization failed");
    }

    solution_ = solver.solve(system.F);
    ; // system.K.partialPivLu().solve(system.F);

    if (solver.info() != Eigen::Success || !solution_.allFinite()) {
      throw std::runtime_error("HeatSolver: linear system solving failed");
    }
  }
  if (method_ == EigenMethod::SIMPLICAL_LDLT) {
    LogDuration timer("solve: SparseLU");
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;

    solver.compute(system.K);
    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("HeatSolver: matrix factorization failed");
    }

    solution_ = solver.solve(system.F);
    ; // system.K.partialPivLu().solve(system.F);

    if (solver.info() != Eigen::Success || !solution_.allFinite()) {
      throw std::runtime_error("HeatSolver: linear system solving failed");
    }
    const double relativeResidual = (system.K * solution_ - system.F).norm() /
                                    std::max(system.F.norm(), 1.0);

    std::cout << "LDLT relative residual: " << relativeResidual << '\n';
  }
}

const Eigen::VectorXd &HeatSolver::Solution() { return solution_; }
} // namespace heat