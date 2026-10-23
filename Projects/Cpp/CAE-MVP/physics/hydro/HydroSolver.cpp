#include "HydroSolver.h"
#include "physics/hydro/assembly/HydroAssembler.h"
#include <iostream>
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
    for (int step = 1; step <= numSteps; ++step) {
        if(step % 10 == 1){
            std::cout<<"step = "<<step<<"\n";
        }
        
        double currTime = step *dt_;
        assembler.UpdateForce(globalSystem, currTime);
        // solve   
    }
    // HeatAssembler2D assembler(problem_, mesh_);
    // LinearSystem system = assembler.Assemble();
    // HeatBoundaryAssembler2D boundaryAssembler;
    // boundaryAssembler.Apply(system, problem_, mesh_);

    // solution_ = system.K.partialPivLu().solve(system.F);

    // if (!solution_.allFinite()) {
    //     throw std::runtime_error("HeatSolver: solution Contains NaN or Inf");
    // }
}

const Eigen::VectorXd &HydroSolver::Solution() { return solution_; }