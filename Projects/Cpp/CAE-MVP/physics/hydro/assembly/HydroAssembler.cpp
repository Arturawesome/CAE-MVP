#include "HydroAssembler.h"

HydroAssembler2D::HydroAssembler2D(const HydroProblem &problem,
                                   const Mesh &mesh,
                                   const TaylorHoodDofMap &dofMap)
    : problem_(problem), mesh_(mesh), dofMap_(dofMap) {}

void HydroAssembler2D::UpdateForce(HydroGlobalSystem &global, int currTime) {
    int numEl = mesh_.NumEl();
    global.force.setZero();
    for (int elId = 0; elId < numEl; ++elId) {
        std::array<Point2D, 3> vertices;

        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
        }

        TriangleP2 triangleP2(vertices[0], vertices[1], vertices[2], elId);

        auto localForce = MakeLocalForce(triangleP2, vertices, currTime);

        for (int i = 0; i < 12; ++i) {
            const int globalI = dofMap_.ElementDofs(elId)[i];
            global.force(globalI) += localForce(i);
        }
    }
}
Eigen::Matrix<double, 12, 1>
HydroAssembler2D::MakeLocalForce(const TriangleP2 &p2,
                                 const std::array<Point2D, 3> &vertices,
                                 double time) const {


    Eigen::Matrix<double, 12, 1> localForce = Eigen::Matrix<double, 12, 1>::Zero();;

    const auto quadraturePoints = MakeQuadraturePoints();

    for (const auto &qp : quadraturePoints) {
        Point2D internalPonitInTrinagle{
            qp.L1 * vertices[0].x + qp.L2 * vertices[1].x +
                qp.L3 * vertices[2].x,
            qp.L1 * vertices[0].y + qp.L2 * vertices[1].y +
                qp.L3 * vertices[2].y};

        const auto velocityShapeVector = p2.Shape(internalPonitInTrinagle);

        // Vector velocity interpolation matrix
        Eigen::Matrix<double, 2, 12> Nu;
        Nu.setZero();
        for (int i = 0; i < 6; ++i) {
            Nu(0, 2 * i) = velocityShapeVector(i);
            Nu(1, 2 * i + 1) = velocityShapeVector(i);
        }
        
        double weight = p2.Area() * qp.weight;        
        Eigen::Vector2d force = problem_.Force(internalPonitInTrinagle.x,
                                               internalPonitInTrinagle.y, time);
        localForce.noalias() += weight * (Nu.transpose() * force);
        
    }
    return localForce;
}

HydroGlobalSystem HydroAssembler2D::AssembleGlobalSystem() {

    HydroGlobalSystem global(dofMap_.numVelocityDofs(),
                             dofMap_.numPressureDofs());

    int numEl = mesh_.NumEl();
    Triplets triplets;
    triplets.massTriplets.reserve(numEl * 12 * 12);
    triplets.viscosityTriplets.reserve(numEl * 12 * 12);
    triplets.divergenceTriplets.reserve(numEl * 3 * 12);

    for (int elId = 0; elId < numEl; ++elId) {
        std::array<Point2D, 3> vertices;

        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
        }

        TriangleP1 triangleP1(mesh_, elId);
        TriangleP2 triangleP2(vertices[0], vertices[1], vertices[2], elId);

        HydroLocalSystem local =
            MakeLocalSystem(triangleP1, triangleP2, vertices, 0);
        AddToGlobal(global, local, dofMap_.ElementDofs(elId),
                    dofMap_.numVelocityDofs(), triplets);
    }
    global.mass.setFromTriplets(triplets.massTriplets.begin(),
                                triplets.massTriplets.end());
    global.viscosity.setFromTriplets(triplets.viscosityTriplets.begin(),
                                     triplets.viscosityTriplets.end());
    global.divergence.setFromTriplets(triplets.divergenceTriplets.begin(),
                                      triplets.divergenceTriplets.end());

    global.mass.makeCompressed();
    global.viscosity.makeCompressed();
    global.divergence.makeCompressed();
    return global;
}

void HydroAssembler2D::AddToGlobal(HydroGlobalSystem &global,
                                   const HydroLocalSystem &local,
                                   const std::array<int, 15> &dofs,
                                   int numVelocityDofs,
                                   Triplets &triplets) const {

    // Velocity operators: 12 x 12
    for (int i = 0; i < 12; ++i) {
        const int globalI = dofs[i];

        // Force vector
        global.force(globalI) += local.force(i);

        for (int j = 0; j < 12; ++j) {
            const int globalJ = dofs[j];

            triplets.massTriplets.emplace_back(globalI, globalJ,
                                               local.mass(i, j));

            triplets.viscosityTriplets.emplace_back(globalI, globalJ,
                                                    local.viscosity(i, j));
        }
    }

    for (int i = 0; i < 3; ++i) {
        const int pressureId = dofs[12 + i] - numVelocityDofs;

        for (int j = 0; j < 12; ++j) {
            const int velocityId = dofs[j];

            triplets.divergenceTriplets.emplace_back(pressureId, velocityId,
                                                     local.divergence(i, j));
        }
    }
}

HydroLocalSystem
HydroAssembler2D::MakeLocalSystem(const TriangleP1 &p1, const TriangleP2 &p2,
                                  const std::array<Point2D, 3> &vertices,
                                  double time) const {

    HydroLocalSystem local;
    local.mass.setZero();
    local.viscosity.setZero();
    local.divergence.setZero();
    local.force.setZero();

    double rho = problem_.Density();
    double mu = problem_.Viscosity();
    // viscosity matrix
    Eigen::Matrix3d C = Eigen::Matrix3d::Zero();
    C(0, 0) = 2.0 * mu;
    C(1, 1) = 2.0 * mu;
    C(2, 2) = mu;

    const auto quadraturePoints = MakeQuadraturePoints();

    for (const auto &qp : quadraturePoints) {
        Point2D internalPonitInTrinagle{
            qp.L1 * vertices[0].x + qp.L2 * vertices[1].x +
                qp.L3 * vertices[2].x,
            qp.L1 * vertices[0].y + qp.L2 * vertices[1].y +
                qp.L3 * vertices[2].y};
        const auto velocityGradMatrix = p2.Grad(internalPonitInTrinagle);
        const auto velocityShapeVector = p2.Shape(internalPonitInTrinagle);

        const auto pressureShape = p1.Shape(internalPonitInTrinagle);

        // Velocity strain-displacement matrix
        Eigen::Matrix<double, 3, 12> B;
        B.setZero();

        // Velocity divergence row
        Eigen::Matrix<double, 1, 12> H;
        H.setZero();

        // Vector velocity interpolation matrix
        Eigen::Matrix<double, 2, 12> Nu;
        Nu.setZero();

        for (int i = 0; i < 6; ++i) {
            const double dNdx = velocityGradMatrix(0, i);
            const double dNdy = velocityGradMatrix(1, i);

            Nu(0, 2 * i) = velocityShapeVector(i);
            Nu(1, 2 * i + 1) = velocityShapeVector(i);

            B(0, 2 * i) = dNdx;
            B(1, 2 * i + 1) = dNdy;

            B(2, 2 * i) = dNdy;
            B(2, 2 * i + 1) = dNdx;

            H(0, 2 * i) = dNdx;
            H(0, 2 * i + 1) = dNdy;
        }
        double weight = p2.Area() * qp.weight;
        local.mass.noalias() += rho * weight * (Nu.transpose() * Nu);
        local.viscosity.noalias() += weight * (B.transpose() * C * B);
        Eigen::Vector2d force = problem_.Force(internalPonitInTrinagle.x,
                                               internalPonitInTrinagle.y, time);
        local.force.noalias() += weight * (Nu.transpose() * force);
        local.divergence.noalias() += weight * (pressureShape * H);
    }
    return local;
}

std::array<TriangleQuadraturePoint, 6>
HydroAssembler2D::MakeQuadraturePoints() const {
    constexpr double quadratureA = 0.816847572980459;
    constexpr double quadratureB = 0.091576213509771;

    constexpr double quadratureC = 0.108103018168070;
    constexpr double quadratureD = 0.445948490915965;

    constexpr double quadratureWeight1 = 0.109951743655322;
    constexpr double quadratureWeight2 = 0.223381589678011;

    std::array<TriangleQuadraturePoint, 6> quadraturePoints = {
        TriangleQuadraturePoint{.L1 = quadratureA,
                                .L2 = quadratureB,
                                .L3 = quadratureB,
                                .weight = quadratureWeight1},
        TriangleQuadraturePoint{.L1 = quadratureB,
                                .L2 = quadratureA,
                                .L3 = quadratureB,
                                .weight = quadratureWeight1},
        TriangleQuadraturePoint{.L1 = quadratureB,
                                .L2 = quadratureB,
                                .L3 = quadratureA,
                                .weight = quadratureWeight1},

        TriangleQuadraturePoint{.L1 = quadratureC,
                                .L2 = quadratureD,
                                .L3 = quadratureD,
                                .weight = quadratureWeight2},
        TriangleQuadraturePoint{.L1 = quadratureD,
                                .L2 = quadratureC,
                                .L3 = quadratureD,
                                .weight = quadratureWeight2},
        TriangleQuadraturePoint{.L1 = quadratureD,
                                .L2 = quadratureD,
                                .L3 = quadratureC,
                                .weight = quadratureWeight2}};
    return quadraturePoints;
}
