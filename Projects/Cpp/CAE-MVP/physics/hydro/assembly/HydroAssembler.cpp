#include "HydroAssembler.h"

HydroAssembler2D::HydroAssembler2D(const HydroProblem &problem,
                                   const mesh::Mesh &mesh,
                                   const fem::TaylorHoodDofMap &dofMap)
    : problem_(problem), mesh_(mesh), dofMap_(dofMap) {}

void HydroAssembler2D::UpdateForce(HydroGlobalSystem &global, double currTime) {
    int numEl = mesh_.NumEl();
    global.force.setZero();
    for (int elId = 0; elId < numEl; ++elId) {
        std::array<geometry::Point2D, 3> vertices;
        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
        }
        fem::TriangleP2 TriangleP2(vertices[0], vertices[1], vertices[2], elId);
        auto localForce = MakeLocalForce(TriangleP2, vertices, currTime);
        for (int i = 0; i < 12; ++i) {
            const int globalI = dofMap_.ElementDofs(elId)[i];
            global.force(globalI) += localForce(i);
        }
    }
    AssembleTraction(global, currTime);
}
void HydroAssembler2D::UpdateConvection(
    HydroGlobalSystem &global, const Eigen::VectorXd &transportVelocity) const {
    if (transportVelocity.size() != dofMap_.numVelocityDofs()) {
        throw std::invalid_argument(
            "HydroAssembler2D: invalid transport velocity size");
    }

    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(mesh_.NumEl() * 12 * 12);

    for (int elId = 0; elId < mesh_.NumEl(); ++elId) {
        std::array<geometry::Point2D, 3> vertices;
        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
        }
        fem::TriangleP2 TriangleP2(vertices[0], vertices[1], vertices[2], elId);
        const auto &dofs = dofMap_.ElementDofs(elId);

        Eigen::Matrix<double, 12, 1> localVelocity;

        for (int i = 0; i < 12; ++i) {
            localVelocity(i) = transportVelocity(dofs[i]);
        }

        auto localConvection =
            MakeLocalConvection(TriangleP2, vertices, localVelocity);
        for(int i = 0; i < 12; i++){
            for(int j = 0; j < 12; ++j){
                triplets.emplace_back(dofs[i], dofs[j], localConvection(i, j));
            }
        }    
    }
    global.convection.setFromTriplets(triplets.begin(), triplets.end());
    global.convection.makeCompressed();
}

Eigen::Matrix<double, 12, 1>
HydroAssembler2D::MakeLocalForce(const fem::TriangleP2 &p2,
                                 const std::array<geometry::Point2D, 3> &vertices,
                                 double time) const {

    Eigen::Matrix<double, 12, 1> localForce =
        Eigen::Matrix<double, 12, 1>::Zero();
    ;
    const auto quadraturePoints = fem::Make6QuadraturePoints();
    for (const auto &qp : quadraturePoints) {
        geometry::Point2D internalPonitInTrinagle{
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

Eigen::Matrix<double, 6, 1>
HydroAssembler2D::MakeLocalTraction(const mesh::BoundaryEdge &edge,
                                    const HydroBoundaryCondition &condition,
                                    double time) const {

    geometry::Point2D normal{0.0, 0.0};
    if (condition.type == HydroBoundaryConditionType::NormalTraction) {
        normal = mesh_.OutwardNormal(edge);
    }
    int id1 = edge.nodes_id.first;
    int id2 = edge.nodes_id.second;

    const geometry::Point2D &p1 = mesh_.NodeById(id1).position;
    const geometry::Point2D &p2 = mesh_.NodeById(id2).position;

    const double dx = p1.x - p2.x;
    const double dy = p1.y - p2.y;
    const double length = std::hypot(dx, dy);
    if (!std::isfinite(length) || length <= 0.0) {
        throw std::invalid_argument("HydroAssembler2D: invalid boundary edge");
    }

    Eigen::Matrix<double, 6, 1> localF = Eigen::Matrix<double, 6, 1>::Zero();
    std::array<fem::EdgeQuadraturePoint, 3> quadro = fem::MakeEdgeQuadraturePoints();
    for (int i = 0; i < 3; ++i) {
        double s = quadro[i].pos;
        double w = quadro[i].weight * length;

        // shape func
        const double N1 = (1 - s) * (1 - 2 * s);
        const double N2 = s * (2 * s - 1);
        const double N3 = 4 * s * (1 - s);

        double x = (1 - s) * p1.x + s * p2.x;
        double y = (1 - s) * p1.y + s * p2.y;

        Eigen::Vector2d traction;
        switch (condition.type) {
        case HydroBoundaryConditionType::Traction: {
            traction = condition.traction(x, y, time);
            break;
        }
        case HydroBoundaryConditionType::NormalTraction: {
            const double tn = condition.normalTraction(x, y, time);
            traction = tn * Eigen::Vector2d(normal.x, normal.y);
            break;
        }
        default:
            throw std::invalid_argument(
                "HydroAssembler2D: unsupported traction type");
        }

        if (!traction.allFinite()) {
            throw std::runtime_error("HydroAssembler2D: non-finite traction");
        }

        localF(0) += w * N1 * traction.x();
        localF(1) += w * N1 * traction.y();

        localF(2) += w * N2 * traction.x();
        localF(3) += w * N2 * traction.y();

        localF(4) += w * N3 * traction.x();
        localF(5) += w * N3 * traction.y();
    }
    return localF;
}

void HydroAssembler2D::AddTractionToGlobal(
    HydroGlobalSystem &global, const mesh::BoundaryEdge &edge,
    const Eigen::Matrix<double, 6, 1> &localForce) const {

    const int id1 = edge.nodes_id.first;
    const int id2 = edge.nodes_id.second;
    std::array<int, 3> ids = {id1, id2, dofMap_.MiddleNodeId(id1, id2)};

    for (int i = 0; i < 3; ++i) {
        int id = ids[i];
        int globIdU = dofMap_.U(id);
        int globIdV = dofMap_.V(id);
        global.force(globIdU) += localForce(2 * i);
        global.force(globIdV) += localForce(2 * i + 1);
    }
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
        std::array<geometry::Point2D, 3> vertices;

        for (int i = 0; i < 3; ++i) {
            vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
        }

        fem::TriangleP1 TriangleP1(vertices[0], vertices[1], vertices[2], elId);
        fem::TriangleP2 TriangleP2(vertices[0], vertices[1], vertices[2], elId);
        HydroLocalSystem local =
            MakeLocalSystem(TriangleP1, TriangleP2, vertices, 0);
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

Eigen::Matrix<double, 12, 12> HydroAssembler2D::MakeLocalConvection(
    const fem::TriangleP2 &p2, const std::array<geometry::Point2D, 3> &vertices,
    const Eigen::Matrix<double, 12, 1>& transportVelocity) const {
    Eigen::Matrix<double, 12, 12> localConvection =
        Eigen::Matrix<double, 12, 12>::Zero();
    double rho = problem_.Density();
    const auto quadraturePoints = fem::Make7QuadraturePoints();

    for (const auto &qp : quadraturePoints) {
        geometry::Point2D internalPonitInTrinagle{
            qp.L1 * vertices[0].x + qp.L2 * vertices[1].x +
                qp.L3 * vertices[2].x,
            qp.L1 * vertices[0].y + qp.L2 * vertices[1].y +
                qp.L3 * vertices[2].y};
        // velocityGradMatrix and shape functions
        const auto gradN = p2.Grad(internalPonitInTrinagle);
        const auto N = p2.Shape(internalPonitInTrinagle);
        const double weight = p2.Area() * qp.weight;

        Eigen::Vector2d beta = Eigen::Vector2d::Zero();

        for (int i = 0; i < 6; i++) {
            beta.x() += N(i) * transportVelocity(2 * i);
            beta.y() += N(i) * transportVelocity(2 * i + 1);
        }

        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                const double derivateve =
                    beta.x() * gradN(0, j) + beta.y() * gradN(1, j);
                const double value = rho * weight * N(i) * derivateve;
                localConvection(2 * i, 2 * j) += value;
                localConvection(2 * i + 1, 2 * j + 1) += value;
            }
        }
    }
    return localConvection;
}

HydroLocalSystem
HydroAssembler2D::MakeLocalSystem(const fem::TriangleP1 &p1, const fem::TriangleP2 &p2,
                                  const std::array<geometry::Point2D, 3> &vertices,
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

    const auto quadraturePoints = fem::Make6QuadraturePoints();
    for (const auto &qp : quadraturePoints) {
        geometry::Point2D internalPonitInTrinagle{
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

void HydroAssembler2D::AssembleTraction(HydroGlobalSystem &global,
                                        double time) const {

    for (const auto &condition : problem_.BoundaryConditions()) {
        if (condition.type != HydroBoundaryConditionType::Traction &&
            condition.type != HydroBoundaryConditionType::NormalTraction) {
            continue;
        }
        for (const auto &edge : mesh_.BoundaryEdges()) {
            if (edge.boundaryId != condition.boundaryId) {
                continue;
            }
            const auto localForce = MakeLocalTraction(edge, condition, time);
            AddTractionToGlobal(global, edge, localForce);
        }
    }
}