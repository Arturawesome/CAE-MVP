#include "HeatSolver.h"

#include "assembly/Assembler2D.h"
#include "assembly/BoundaryAssembler2D.h"
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

HeatSolver::HeatSolver(HeatProblem &problem)
    : problem_(problem), mesh_(problem.GetMesh()) {

    std::cout << "HeatSolver::HeatSolver\n";
    int numNodes = mesh_.NumNodes();
    solution_ = Eigen::VectorXd::Zero(numNodes);
}

void HeatSolver::Solve() {

    HeatAssembler2D assembler(problem_, mesh_);
    LinearSystem system = assembler.Assemble();
    HeatBoundaryAssembler2D boundaryAssembler;
    boundaryAssembler.Apply(system, problem_, mesh_);

    solution_ = system.K.partialPivLu().solve(system.F);

    if (!solution_.allFinite()) {
        throw std::runtime_error("HeatSolver: solution Contains NaN or Inf");
    }
}

const Eigen::VectorXd &HeatSolver::Solution() { return solution_; }