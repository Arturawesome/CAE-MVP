#include "Problem.h"

HeatProblem::HeatProblem(const mesh::Mesh &mesh, double lambda,
                         std::function<double(double, double, double)> source)
    : mesh_(mesh), lambda_(lambda), source_(source) {
    std::cout << "HeatProblem::HeatProblem\n";
    if (lambda_ <= 0) {
        throw std::invalid_argument("HeatProblem: lambda must be positive\n");
    }
}

double HeatProblem::Lambda() const { return lambda_; }

double HeatProblem::Source(double x, double y, double z) const {
    return source_(x, y, z);
}
double HeatProblem::Source(const geometry::Point2D &elementCenter) const {
    return source_(elementCenter.x, elementCenter.y, 0);
}

const mesh::Mesh &HeatProblem::GetMesh() const { return mesh_; }

void HeatProblem::AddDirichlet(BoundaryId id, double temprature) {
    HeatBoundaryConditions_.push_back(
        HeatBoundaryCondition{.boundaryId = id,
                          .type = HeatBoundaryType::Dirichlet,
                          .value1 = temprature,
                          .value2 = 0});
}
void HeatProblem::AddNeumann(BoundaryId id, double flux) {
    HeatBoundaryConditions_.push_back(
        HeatBoundaryCondition{.boundaryId = id,
                          .type = HeatBoundaryType::Neumann,
                          .value1 = flux,
                          .value2 = 0});
}
void HeatProblem::AddRobin(BoundaryId id, double alpha, double temprature) {
    HeatBoundaryConditions_.push_back(
        HeatBoundaryCondition{.boundaryId = id,
                          .type = HeatBoundaryType::Robin,
                          .value1 = alpha,
                          .value2 = temprature});
}

const std::vector<HeatBoundaryCondition> &HeatProblem::HeatBoundaryConditions() const {
    return HeatBoundaryConditions_;
}
