#include "Solver.h"

#include "io/VTKSeriesWriter.h"
#include "physics/deformation/assembly/BoundaryAssembler.h"

#include <Eigen/SparseLU>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <string>

namespace deformation {
namespace {

constexpr double beta = 0.25;
constexpr double gamma = 0.5;

Eigen::VectorXd SolveSparse(const fem::LinearSystem& system) {
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.analyzePattern(system.K);
    solver.factorize(system.K);
    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("Deformation Solver: matrix factorization failed");
    }
    Eigen::VectorXd x = solver.solve(system.F);
    if (solver.info() != Eigen::Success || !x.allFinite()) {
        throw std::runtime_error("Deformation Solver: linear solve failed");
    }
    return x;
}

// VTK legacy: original coordinates + point displacement/velocity, so ParaView
// can use Warp By Vector with a user-selected scale factor.
void WriteFrame(const mesh::Mesh& mesh, const Eigen::VectorXd& u,
                const Eigen::VectorXd& v, const std::filesystem::path& path) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("Deformation Solver: cannot write VTK file");
    out << std::setprecision(17);
    out << "# vtk DataFile Version 3.0\nLinear elasticity\nASCII\nDATASET UNSTRUCTURED_GRID\n";
    out << "POINTS " << mesh.NumNodes() << " double\n";
    for (const auto& node : mesh.Nodes()) {
        out << node.position.x << ' ' << node.position.y << " 0\n";
    }
    out << "CELLS " << mesh.NumEl() << ' ' << 4 * mesh.NumEl() << "\n";
    for (const auto& el : mesh.Elements()) {
        out << "3 " << el.nodes_id[0] << ' ' << el.nodes_id[1] << ' '
            << el.nodes_id[2] << '\n';
    }
    out << "CELL_TYPES " << mesh.NumEl() << "\n";
    for (int i = 0; i < mesh.NumEl(); ++i) out << "5\n"; // VTK_TRIANGLE
    out << "POINT_DATA " << mesh.NumNodes() << "\n";
    out << "VECTORS Displacement double\n";
    for (int i = 0; i < mesh.NumNodes(); ++i)
        out << u(2 * i) << ' ' << u(2 * i + 1) << " 0\n";
    out << "VECTORS Velocity double\n";
    for (int i = 0; i < mesh.NumNodes(); ++i)
        out << v(2 * i) << ' ' << v(2 * i + 1) << " 0\n";
    if (!out) throw std::runtime_error("Deformation Solver: VTK write failed");
}

} // namespace

Solver::Solver(const Problem& problem, const mesh::Mesh& mesh,
               double timeEnd, double dt)
    : problem_(problem), mesh_(mesh), timeEnd_(timeEnd), dt_(dt) {
    if (!std::isfinite(dt_) || !std::isfinite(timeEnd_) ||
        dt_ <= 0.0 || timeEnd_ <= 0.0 || mesh_.NumNodes() == 0) {
        throw std::invalid_argument("Deformation Solver: invalid time or mesh");
    }
    const Eigen::Index n = 2 * mesh_.NumNodes();
    displacement_ = Eigen::VectorXd::Zero(n);
    velocity_ = Eigen::VectorXd::Zero(n);
    acceleration_ = Eigen::VectorXd::Zero(n);
}

fem::LinearSystem Solver::BuildTimeSystem(
    const GlobalSystem& global,
    const Eigen::VectorXd& predictedDisplacement,
    double stepDt) const {
    const double coefficient = 1.0 / (beta * stepDt * stepDt);
    fem::LinearSystem result(static_cast<int>(global.force.size()));
    result.K = global.stiffness + coefficient * global.mass;
    result.K.makeCompressed();
    result.F = global.force + coefficient * (global.mass * predictedDisplacement);
    return result;
}

void Solver::Solve(const std::string& directory) {
    Assembler assembler(problem_, mesh_);
    BoundaryAssembler2D boundary(problem_, mesh_);
    GlobalSystem global = assembler.AssembleGlobalSystem();

    displacement_.setZero();
    velocity_.setZero();
    acceleration_.setZero();


    const auto initialConstraints = boundary.CollectDisplacementDirichlet(0.0);
    for (const auto& [dof, value] : initialConstraints) {
        if (std::abs(value) > 1e-12) {
            throw std::invalid_argument(
                "Deformation Solver: nonzero Dirichlet displacement at t=0; "
                "initial displacement is zero");
        }
        displacement_(dof) = value;
    }

    assembler.UpdateTimeDependentData(global, 0.0);
    boundary.AddTraction(global, 0.0);

    
    fem::LinearSystem initial(static_cast<int>(global.force.size()));
    initial.K = global.mass;
    initial.F = global.force - global.stiffness * displacement_;

    {
        Eigen::SparseMatrix<double> mass = initial.K;
        std::vector<Eigen::Triplet<double>> entries;
        std::vector<char> fixed(static_cast<std::size_t>(initial.F.size()), false);
        for (const auto& [dof, value] : initialConstraints) {
            (void)value;
            fixed[static_cast<std::size_t>(dof)] = true;
            initial.F(dof) = 0.0;
        }
        for (int col = 0; col < mass.outerSize(); ++col)
            for (Eigen::SparseMatrix<double>::InnerIterator it(mass, col); it; ++it)
                if (!fixed[static_cast<std::size_t>(it.row())] &&
                    !fixed[static_cast<std::size_t>(col)])
                    entries.emplace_back(it.row(), col, it.value());
        for (const auto& [dof, value] : initialConstraints) {
            (void)value;
            entries.emplace_back(dof, dof, 1.0);
        }
        initial.K.setFromTriplets(entries.begin(), entries.end());
    }
    acceleration_ = SolveSparse(initial);

    VTKSeriesWriter series(directory);
    const auto save = [&](int frame, double time) {
        std::ostringstream name;
        name << "deformation_" << std::setw(5) << std::setfill('0')
             << frame << ".vtk";
        WriteFrame(mesh_, displacement_, velocity_,
                   std::filesystem::path(directory) / name.str());
        series.AddFrame(name.str(), time);
    };
    save(0, 0.0);

    double time = 0.0;
    int step = 0;
    while (time < timeEnd_) {
        const double h = std::min(dt_, timeEnd_ - time);
        if (!(h > 0.0)) break;
        const double nextTime = time + h;

        assembler.UpdateTimeDependentData(global, nextTime);
        boundary.AddTraction(global, nextTime);

        const Eigen::VectorXd predictedU =
            displacement_ + h * velocity_ +
            h * h * (0.5 - beta) * acceleration_;
        const Eigen::VectorXd predictedV =
            velocity_ + h * (1.0 - gamma) * acceleration_;

        fem::LinearSystem system = BuildTimeSystem(global, predictedU, h);
        boundary.ApplyDirichlet(system, nextTime);
        Eigen::VectorXd nextU = SolveSparse(system);
        Eigen::VectorXd nextA = (nextU - predictedU) / (beta * h * h);
        Eigen::VectorXd nextV = predictedV + gamma * h * nextA;

        displacement_.swap(nextU);
        velocity_.swap(nextV);
        acceleration_.swap(nextA);
        time = nextTime;
        ++step;

        if (step % 10 == 0 || time >= timeEnd_) {
            save(step, time);
            std::cout << "Deformation step " << step
                      << ", time=" << time
                      << ", max|u|=" << displacement_.cwiseAbs().maxCoeff()
                      << '\n';
        }
    }
    series.WriteSeries();
}

const Eigen::VectorXd& Solver::Solution() const { return displacement_; }
const Eigen::VectorXd& Solver::Velocity() const { return velocity_; }
const Eigen::VectorXd& Solver::Acceleration() const { return acceleration_; }

} // namespace deformation
