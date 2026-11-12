#include "Assembler2D.h"
#include <thread>

namespace heat {
HeatAssembler2D::HeatAssembler2D(const HeatProblem &problem,
                                 const mesh::Mesh &mesh,
                                 unsigned int threadCount)
    : problem_(problem), mesh_(mesh), threadCount_(threadCount) {}

fem::LinearSystem HeatAssembler2D::AssemblePar() {
  int numEl = mesh_.NumEl();

  fem::LinearSystem system = fem::LinearSystem(mesh_.NumNodes());

  std::vector<Eigen::Triplet<double>> triplets(
      static_cast<std::size_t>(mesh_.NumEl()) * 9);

  std::vector<Eigen::Vector3d> localForces(numEl);

  std::vector<std::jthread> threads;
  threads.reserve(threadCount_);
  for (size_t i = 0; i < threadCount_; ++i) {
    const std::size_t begin = i * numEl / threadCount_;
    const std::size_t end = (i + 1) * numEl / threadCount_;
    threads.emplace_back([begin, end, numEl, &triplets, &localForces, this]() {
      AssembleRange(begin, end, numEl, triplets, localForces);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }
  for (int elId = 0; elId < numEl; ++elId) {
    for (int i = 0; i < 3; ++i) {
      const int nodeId = mesh_.NodeIdFromEl(elId, i);

      system.F(nodeId) += localForces[elId](i);
    }
  }

  system.K.setFromTriplets(triplets.begin(), triplets.end());
  system.K.makeCompressed();
  return system;
}

void HeatAssembler2D::AssembleRange(
    int begin, int end, int numEl,
    std::vector<Eigen::Triplet<double>> &triplets,
    std::vector<Eigen::Vector3d> &localForsec) {
  for (int elId = begin; elId < std::min(end, numEl); ++elId) {
    std::array<geometry::Point2D, 3> vertices;
    for (int i = 0; i < 3; ++i) {
      vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
    }

    fem::TriangleP1 triangle(vertices[0], vertices[1], vertices[2], elId);
    localForsec[elId] = std::move(MakeLocalF(triangle));
    auto locK = MakeLocalK(triangle);

    for (std::size_t i = 0; i < 3; ++i) {
      int node_i_id = mesh_.NodeIdFromEl(elId, i);
      for (std::size_t j = 0; j < 3; ++j) {
        int node_j_id = mesh_.NodeIdFromEl(elId, j);
        triplets[9 * elId + 3 * i + j] =
            Eigen::Triplet<double>(node_i_id, node_j_id, locK(i, j));
      }
    }
  }
}

fem::LinearSystem HeatAssembler2D::Assemble() {
  fem::LinearSystem system = fem::LinearSystem(mesh_.NumNodes());
  int numEl = mesh_.NumEl();
  std::vector<Eigen::Triplet<double>> triplets;
  triplets.reserve(static_cast<std::size_t>(mesh_.NumEl()) * 9);
  for (int elId = 0; elId < numEl; ++elId) {
    std::array<geometry::Point2D, 3> vertices;
    for (int i = 0; i < 3; ++i) {
      vertices[i] = mesh_.NodeById(mesh_.NodeIdFromEl(elId, i)).position;
    }
    fem::TriangleP1 triangle(vertices[0], vertices[1], vertices[2], elId);

    auto locF = MakeLocalF(triangle);
    auto locK = MakeLocalK(triangle);

    // collect triplets
    for (std::size_t i = 0; i < 3; ++i) {
      int node_i_id = mesh_.NodeIdFromEl(elId, i);

      for (std::size_t j = 0; j < 3; ++j) {
        int node_j_id = mesh_.NodeIdFromEl(elId, j);
        triplets.emplace_back(node_i_id, node_j_id, locK(i, j));
      }
      // collect forsec
      system.F(node_i_id) += locF(i);
    }
  }
  system.K.setFromTriplets(triplets.begin(), triplets.end());
  system.K.makeCompressed();
  return system;
}

Eigen::Vector3d
HeatAssembler2D::MakeLocalF(const fem::TriangleP1 &triangle) const {
  Eigen::Vector3d locF_;

  double source = problem_.Source(triangle.Center()) / 3 * triangle.Area();
  for (std::size_t i = 0; i < 3; ++i) {
    locF_(i) = source;
  }
  return locF_;
}

Eigen::Matrix<double, 3, 3>
HeatAssembler2D::MakeLocalK(const fem::TriangleP1 &triangle) const {
  Eigen::Matrix<double, 3, 3> locK_;

  double lambda = problem_.Lambda();
  locK_ =
      lambda * triangle.Area() * triangle.Grad().transpose() * triangle.Grad();
  return locK_;
}
} // namespace heat