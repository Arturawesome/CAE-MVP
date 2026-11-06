#include "Solver.h"
#include "physics/deformation/assembly/Assembler.h"
#include <iostream>

namespace deformation {
Solver::Solver(const Problem &problem, mesh::Mesh &mesh, double timeEnd,
               double dt)
    : problem_(problem), mesh_(mesh), timeEnd_(timeEnd), dt_(dt) {

  solution_ = Eigen::VectorXd::Zero(2 * mesh_.NumNodes());
  if (dt_ <= 0.0 || timeEnd_ <= 0.0) {
    throw std::invalid_argument("deformation::Solver: invalid time parameters");
  }
  std::cout << "deformation::solver\n";
}

fem::LinearSystem Solver::BuildTimeSystem() const {}

void Solver::Solve(std::string filename) {
  // Assembler2D assembler(problem_, mesh_, mesh_);
  const int numSteps = static_cast<int>(std::ceil(timeEnd_ / dt_));

  Assembler assembler(problem_, mesh_);

  GlobalSystem global = assembler.AssembleGlobalSystem();

  // Начальное состояние
  Eigen::VectorXd displacement = Eigen::VectorXd::Zero(global.force.size());
  Eigen::VectorXd velocity = Eigen::VectorXd::Zero(global.force.size());
  Eigen::VectorXd acceleration = Eigen::VectorXd::Zero(global.force.size());

  Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
  for (int step = 1; step <= numSteps; ++step) {
    double currTime = step * dt_;
    assembler.UpdateTimeDependentData(global, currTime);

    // 1. BoundaryAssembler добавляет поверхностные нагрузки.
    //  2. Solver строит эффективную систему Ньюмарка.
    //  3. BoundaryAssembler накладывает условия Дирихле.
    //  4. Решаем систему.
    //  5. Обновляем displacement, velocity, acceleration.
    if (step % 10 == 0) {
      // saveFrame(step, currTime);
      std::cout << "step = " << step << "\n";
    }
  }
  // seriesWriter.WriteSeries();
}

const Eigen::VectorXd &Solver::Solution() { return solution_; }

} // namespace deformation