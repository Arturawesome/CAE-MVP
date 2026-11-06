#include "HeatSolver.h"

#include "assembly/Assembler2D.h"
#include "assembly/BoundaryAssembler2D.h"
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace heat {
HeatSolver::HeatSolver(HeatProblem &problem)
    : problem_(problem), mesh_(problem.GetMesh()) {

    std::cout << "HeatSolver::HeatSolver\n";
    int numNodes = mesh_.NumNodes();
    solution_ = Eigen::VectorXd::Zero(numNodes);
}

void HeatSolver::Solve() {

    HeatAssembler2D assembler(problem_, mesh_);
    fem::LinearSystem system = assembler.Assemble();
    HeatBoundaryAssembler2D boundaryAssembler;
    boundaryAssembler.Apply(system, problem_, mesh_);
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

const Eigen::VectorXd &HeatSolver::Solution() { return solution_; }
} // namespace heat