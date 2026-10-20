#pragma once
#include <Eigen/Dense>

#include "fem/dofs/P2DofMap.h"
class HydroSolver {
    private:
        const P2DofMap& p2DofMap_;
        //const HydroProblem& problem_;
        Eigen::VectorXd solution_;
    public:
        explicit HydroSolver(const P2DofMap& p2DofMap); //,const HydroProblem& problem etc...
        ~HydroSolver() = default;
};