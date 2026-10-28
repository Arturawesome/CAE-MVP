#pragma once

#include <string>

#include <Eigen/Dense>

#include "mesh/Mesh2D.h"
#include "fem/dofs/TaylorHoodDofMap.h"

class VTKHydroWriter {
public:
    static void Write(
        const Mesh& mesh,
        const TaylorHoodDofMap& dofMap,
        const Eigen::VectorXd& solution,
        const std::string& filename
    );
};