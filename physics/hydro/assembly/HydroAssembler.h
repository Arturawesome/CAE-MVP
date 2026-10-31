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
namespace hydro {
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
    Eigen::SparseMatrix<double> convection;
    Eigen::VectorXd force;

    HydroGlobalSystem(int numVelocityDofs, int numPressureDofs)
        : mass(numVelocityDofs, numVelocityDofs),
          viscosity(numVelocityDofs, numVelocityDofs),
          divergence(numPressureDofs, numVelocityDofs),
          convection(numVelocityDofs, numVelocityDofs),
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
    const mesh::Mesh &mesh_;
    const fem::TaylorHoodDofMap &dofMap_;

  public:
    HydroAssembler2D(const HydroProblem &problem, const mesh::Mesh &mesh,
                     const fem::TaylorHoodDofMap &dofMap);
    HydroGlobalSystem AssembleGlobalSystem();
    void UpdateForce(HydroGlobalSystem &global, double currTime);
    void UpdateConvection(HydroGlobalSystem &global,
                          const Eigen::VectorXd &transportVelocity) const;

  private:
    Eigen::Matrix<double, 12, 12> MakeLocalConvection(
        const fem::TriangleP2 &p2,
        const std::array<geometry::Point2D, 3> &vertices,
        const Eigen::Matrix<double, 12, 1> &transportVelocity) const;

    Eigen::Matrix<double, 12, 1>
    MakeLocalForce(const fem::TriangleP2 &p2,
                   const std::array<geometry::Point2D, 3> &vertices,
                   double time) const;
    void AddToGlobal(HydroGlobalSystem &global, const HydroLocalSystem &local,
                     const std::array<int, 15> &dofs, int numVelocityDofs,
                     Triplets &triplets) const;
    HydroLocalSystem
    MakeLocalSystem(const fem::TriangleP1 &p1, const fem::TriangleP2 &p2,
                    const std::array<geometry::Point2D, 3> &vertices,
                    double time) const;
    // std::array<TriangleQuadraturePoint, 7>
    // MakeConvectionQuadraturePoints() const;
    // std::array<TriangleQuadraturePoint, 6> MakeQuadraturePoints() const;
    // std::array<EdgeQuadraturePoint, 3> MakeEdgeQuadraturePoints() const;

    Eigen::Matrix<double, 6, 1>
    MakeLocalTraction(const mesh::BoundaryEdge &edge,
                      const HydroBoundaryCondition &condition,
                      double time) const;

    void
    AddTractionToGlobal(HydroGlobalSystem &global,
                        const mesh::BoundaryEdge &edge,
                        const Eigen::Matrix<double, 6, 1> &localForce) const;

    // обход по граничным ребрам
    void AssembleTraction(HydroGlobalSystem &global, double time) const;
};

} // namespace hydro