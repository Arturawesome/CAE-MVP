#include "BoundaryAssembler.h"

#include <Eigen/Sparse>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace deformation {

namespace {

bool IsDirichlet(BoundaryConditionType type) {
  return type == BoundaryConditionType::DisplacementDirichlet ||
         type == BoundaryConditionType::DisplacementX ||
         type == BoundaryConditionType::DisplacementY;
}

bool IsTraction(BoundaryConditionType type) {
  return type == BoundaryConditionType::Traction ||
         type == BoundaryConditionType::NormalTraction;
}

void InsertConstraint(std::map<int, double> &constraints, int dof,
                      double value) {
  if (!std::isfinite(value)) {
    throw std::invalid_argument(
        "Deformation BC: non-finite prescribed displacement");
  }
  auto [it, inserted] = constraints.emplace(dof, value);
  if (!inserted) {
    const double scale = std::max({1.0, std::abs(value), std::abs(it->second)});
    if (std::abs(it->second - value) > 1e-12 * scale) {
      throw std::invalid_argument(
          "Deformation BC: conflicting displacement at DOF " +
          std::to_string(dof));
    }
  }
}

} // namespace

BoundaryAssembler2D::BoundaryAssembler2D(const Problem &problem,
                                         const mesh::Mesh &mesh)
    : problem_(problem), mesh_(mesh) {}

void BoundaryAssembler2D::AddTraction(GlobalSystem &global, double time) const {
  const int numDofs = 2 * mesh_.NumNodes();
  if (global.force.size() != numDofs) {
    throw std::invalid_argument("Deformation BC: incorrect global force size");
  }

  // Two-point Gauss rule on s in [0,1].
  constexpr double s[2] = {0.21132486540518713, 0.78867513459481287};
  constexpr double w[2] = {0.5, 0.5};

  for (const auto &bc : problem_.BoundaryConditions()) {
    if (!IsTraction(bc.type))
      continue;

    for (const auto &edge : mesh_.BoundaryEdges()) {
      if (edge.boundaryId != bc.boundaryId)
        continue;

      const int aId = edge.nodes_id.first;
      const int bId = edge.nodes_id.second;
      const auto &a = mesh_.NodeById(aId).position;
      const auto &b = mesh_.NodeById(bId).position;
      const double length = std::hypot(b.x - a.x, b.y - a.y);
      if (!(length > 0.0) || !std::isfinite(length)) {
        throw std::invalid_argument("Deformation BC: invalid boundary edge");
      }

      Eigen::Vector2d normal = Eigen::Vector2d::Zero();
      if (bc.type == BoundaryConditionType::NormalTraction) {
        const auto n = mesh_.OutwardNormal(edge);
        normal = Eigen::Vector2d(n.x, n.y);
        if (!bc.normalTraction) {
          throw std::invalid_argument(
              "Deformation BC: empty normal traction callback");
        }
      } else if (!bc.traction) {
        throw std::invalid_argument("Deformation BC: empty traction callback");
      }

      for (int q = 0; q < 2; ++q) {
        const double nA = 1.0 - s[q];
        const double nB = s[q];
        const double x = nA * a.x + nB * b.x;
        const double y = nA * a.y + nB * b.y;

        const Eigen::Vector2d traction =
            bc.type == BoundaryConditionType::Traction
                ? bc.traction(x, y, time)
                : bc.normalTraction(x, y, time) * normal;
        if (!traction.allFinite()) {
          throw std::invalid_argument("Deformation BC: non-finite traction");
        }

        const double weight = length * w[q];
        global.force.segment<2>(2 * aId) += weight * nA * traction;
        global.force.segment<2>(2 * bId) += weight * nB * traction;
      }
    }
  }
}

BoundaryAssembler2D::PrescribedDofs
BoundaryAssembler2D::CollectDisplacementDirichlet(double time) const {
  PrescribedDofs prescribed;

  for (const auto &bc : problem_.BoundaryConditions()) {
    if (!IsDirichlet(bc.type))
      continue;

    for (const auto &edge : mesh_.BoundaryEdges()) {
      if (edge.boundaryId != bc.boundaryId)
        continue;

      for (int nodeId : {edge.nodes_id.first, edge.nodes_id.second}) {
        const auto &p = mesh_.NodeById(nodeId).position;
        switch (bc.type) {
        case BoundaryConditionType::DisplacementDirichlet: {
          if (!bc.displacement) {
            throw std::invalid_argument(
                "Deformation BC: empty displacement callback");
          }
          const Eigen::Vector2d value = bc.displacement(p.x, p.y, time);
          InsertConstraint(prescribed, 2 * nodeId, value.x());
          InsertConstraint(prescribed, 2 * nodeId + 1, value.y());
          break;
        }
        case BoundaryConditionType::DisplacementX:
        case BoundaryConditionType::DisplacementY: {
          if (!bc.displacementComponent) {
            throw std::invalid_argument(
                "Deformation BC: empty component callback");
          }
          const int component =
              bc.type == BoundaryConditionType::DisplacementX ? 0 : 1;
          InsertConstraint(prescribed, 2 * nodeId + component,
                           bc.displacementComponent(p.x, p.y, time));
          break;
        }
        default:
          break;
        }
      }
    }
  }
  return prescribed;
}

void BoundaryAssembler2D::ApplyDirichlet(fem::LinearSystem &system,
                                         double time) const {
  const int n = 2 * mesh_.NumNodes();
  if (system.K.rows() != n || system.K.cols() != n || system.F.size() != n) {
    throw std::invalid_argument(
        "Deformation BC: invalid effective system dimensions");
  }

  const PrescribedDofs prescribed = CollectDisplacementDirichlet(time);
  if (prescribed.empty())
    return;

  // Keep the original system unchanged until the transformation is complete.
  Eigen::SparseMatrix<double> original = system.K;
  original.makeCompressed();
  Eigen::VectorXd rhs = system.F;
  std::vector<char> fixed(static_cast<std::size_t>(n), false);
  for (const auto &[dof, value] : prescribed) {
    fixed[static_cast<std::size_t>(dof)] = true;
  }

  // Correct the RHS for all eliminated columns: F_free -= K_free,fixed * g.
  std::vector<Eigen::Triplet<double>> triplets;
  triplets.reserve(static_cast<std::size_t>(original.nonZeros()) +
                   prescribed.size());
  for (int col = 0; col < original.outerSize(); ++col) {
    const auto constraint = prescribed.find(col);
    const bool fixedCol = constraint != prescribed.end();
    for (Eigen::SparseMatrix<double>::InnerIterator it(original, col); it;
         ++it) {
      const int row = it.row();
      if (!fixed[static_cast<std::size_t>(row)] && fixedCol) {
        rhs(row) -= it.value() * constraint->second;
      }
      if (!fixed[static_cast<std::size_t>(row)] && !fixedCol) {
        triplets.emplace_back(row, col, it.value());
      }
    }
  }

  for (const auto &[dof, value] : prescribed) {
    triplets.emplace_back(dof, dof, 1.0);
    rhs(dof) = value;
  }

  Eigen::SparseMatrix<double> constrained(n, n);
  constrained.setFromTriplets(triplets.begin(), triplets.end());
  constrained.makeCompressed();
  system.K.swap(constrained);
  system.F.swap(rhs);
}

} // namespace deformation
