#include "FEMSolver.h"

#include "assembly/HeatAssembler2D.h"
#include "assembly/HeatBoundaryAssembler2D.h"
#include "elements/TriangleP1.h"
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

FEMSolver::FEMSolver(HeatProblem &problem_)
    : problem(problem_), mesh(problem_.GetMesh()) {

    std::cout << "FEMSolver::FEMSolver\n";
    int numNodes = mesh.NumNodes();
    solution = Eigen::VectorXd::Zero(numNodes);
}

void FEMSolver::Solve() {

    HeatAssembler2D assembler(problem, mesh);
    LinearSystem system = assembler.Assemble();
    HeatBoundaryAssembler2D boundaryAssembler;
    boundaryAssembler.Apply(system, problem, mesh);

    solution = system.K.partialPivLu().solve(system.F);

    if (!solution.allFinite()) {
        throw std::runtime_error("FEMSolver: solution Contains NaN or Inf");
    }
}

const Eigen::VectorXd &FEMSolver::Solution() { return solution; }