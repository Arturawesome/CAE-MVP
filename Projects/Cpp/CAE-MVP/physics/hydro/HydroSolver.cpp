#include "HydroSolver.h"
#include "physics/hydro/assembly/HydroAssembler.h"
#include <iostream>
#include <vector>
#include <Eigen/Sparse>

HydroSolver::HydroSolver(const HydroProblem &problem, Mesh &mesh,
                         double timeEnd, double dt)
    : problem_(problem), mesh_(mesh), timeEnd_(timeEnd), dt_(dt),
      p2DofMap_(mesh_), taylorHoodDofMap_(mesh_, p2DofMap_) {

    solution_ = Eigen::VectorXd::Zero(taylorHoodDofMap_.NumDofs());
    if (dt_ <= 0.0 || timeEnd_ <= 0.0) {
        throw std::invalid_argument("HydroSolver: invalid time parameters");
    }
}

void HydroSolver::Solve() {

    HydroAssembler2D assembler(problem_, mesh_, taylorHoodDofMap_);

    auto globalSystem = assembler.AssembleGlobalSystem();
    const int numSteps = static_cast<int>(std::ceil(timeEnd_ / dt_));
    double prevTime = 0;
    auto currVelocity = MakeInitialVelocity();
    solution_.head(currVelocity.size()) = currVelocity;
    for (int step = 1; step <= numSteps; ++step) {
        if (step % 10 == 1) {
            std::cout << "step = " << step << "\n";
        }
        double currTime = step * dt_;
        assembler.UpdateForce(globalSystem, currTime);
        // build Time system
        const Eigen::VectorXd previousVelocity = solution_.head(globalSystem.mass.rows());
        LinearSystem system = BuildTimeSystem(globalSystem, previousVelocity, dt_);
        // Apply Boundary Conditions

        // Solver for lin system

        //  new velosity

        prevTime = currTime;
    }
}
Eigen::VectorXd HydroSolver::MakeInitialVelocity() const {
    const int numNodes = p2DofMap_.NumP2Nodes();
    Eigen::VectorXd velocity =  Eigen::VectorXd::Zero(2*numNodes);
    for(int nodeId = 0; nodeId < numNodes; nodeId++){
        auto point = p2DofMap_.P2NodeById(nodeId);

        auto nodeVelxy = problem_.InitialVelocity(point.position.x, point.position.y);
         velocity(taylorHoodDofMap_.U(nodeId)) =
            nodeVelxy.x();

        velocity(taylorHoodDofMap_.V(nodeId)) =
            nodeVelxy.y();
    }
    return velocity;
}

LinearSystem
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

        throw std::invalid_argument(
            "HydroSolver: incompatible matrix dimensions");
    }

    LinearSystem system(numDofs);
    std::vector<Triplet> triplets;
    triplets.reserve(global.mass.nonZeros() + global.viscosity.nonZeros() +
                     2 * global.divergence.nonZeros());

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
    system.K.setFromTriplets(triplets.begin(), triplets.end());
    system.K.makeCompressed();
    system.F.head(numVelocityDofs) = global.force + (global.mass * previousVelocity) / dt;
    return system;
}

const Eigen::VectorXd &HydroSolver::Solution() { return solution_; }