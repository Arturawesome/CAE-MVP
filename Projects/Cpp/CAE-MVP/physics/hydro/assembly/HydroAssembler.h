#pragma once
#include "fem/dofs/P2DofMap.h"
#include "fem/dofs/TaylorHoodDofMap.h"
#include "fem/elements/TriangleP1.h"
#include "fem/elements/TriangleP2.h"
#include "fem/systems/LinearSystem.h"
#include "mesh/Mesh2D.h"
#include "physics/hydro/problem/HydroProblem.h"
#include <Eigen/Sparse>
#include <array>
#include <vector>

using SparseTriplet = Eigen::Triplet<double>;

// block numeration: global solition vector:
// [vx0, vx1, ... vxN, vy0, vy1.. vyN, p1, p2, ... pN]
// For one elements
struct HydroLocalSystem {
    Eigen::Matrix<double, 12, 12> mass;
    Eigen::Matrix<double, 12, 12> viscosity;
    Eigen::Matrix<double, 3, 12> divergence;
    Eigen::Matrix<double, 12, 1> force;
};

struct HydroGlobalSystem {
    Eigen::SparseMatrix<double> mass;
    Eigen::SparseMatrix<double> viscosity;
    Eigen::SparseMatrix<double> divergence;
    Eigen::VectorXd force;
    HydroGlobalSystem(int numVelocityDofs, int numPressureDofs)
        : mass(numVelocityDofs, numVelocityDofs),
          viscosity(numVelocityDofs, numVelocityDofs),
          divergence(numPressureDofs, numVelocityDofs),
          force(Eigen::VectorXd::Zero(numVelocityDofs)) {}
};

struct Triplets {
    std::vector<SparseTriplet> massTriplets;
    std::vector<SparseTriplet> viscosityTriplets;
    std::vector<SparseTriplet> divergenceTriplets;
};

class HydroAssembler2D {

  private:
    const HydroProblem &problem_;
    const Mesh &mesh_;
    const TaylorHoodDofMap &dofMap_;

  public:
    HydroAssembler2D(const HydroProblem &problem, const Mesh &mesh,
                     const TaylorHoodDofMap &dofMap);
    HydroGlobalSystem AssembleGlobalSystem();
    void UpdateForce(HydroGlobalSystem &global, double currTime);

  private:
    Eigen::Matrix<double, 12, 1>
    MakeLocalForce(const TriangleP2 &p2, const std::array<Point2D, 3> &vertices,
                   double time) const;
    void AddToGlobal(HydroGlobalSystem &global, const HydroLocalSystem &local,
                     const std::array<int, 15> &dofs, int numVelocityDofs,
                     Triplets &triplets) const;
    HydroLocalSystem MakeLocalSystem(const TriangleP1 &p1, const TriangleP2 &p2,
                                     const std::array<Point2D, 3> &vertices,
                                     double time) const;
    std::array<TriangleQuadraturePoint, 6> MakeQuadraturePoints() const;

    Eigen::Matrix<double, 6, 1>
    MakeLocalTraction(const BoundaryEdge &edge,
                      const HydroBoundaryCondition &condition,
                      double time) const;

    void
    AddTractionToGlobal(HydroGlobalSystem &global, const BoundaryEdge &edge,
                        const Eigen::Matrix<double, 6, 1> &localForce) const;

    void AssembleTraction(HydroGlobalSystem &global, double time) const;
};