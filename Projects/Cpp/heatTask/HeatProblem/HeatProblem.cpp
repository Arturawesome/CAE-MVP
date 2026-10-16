#include "HeatProblem.h"

HeatProblem::HeatProblem(const Mesh &mesh_, double lambda_,
                         std::function<double(double, double, double)> source_)
    : mesh(mesh_), lambda(lambda_), source(source_) {
    std::cout << "HeatProblem::HeatProblem\n";
    if (lambda_ <= 0) {
        throw std::invalid_argument("HeatProblem: lambda must be positive\n");
    }
}

double HeatProblem::GetLambda() const { return lambda; }

double HeatProblem::GetSource(double x, double y, double z) const {
    return source(x, y, z);
}
double HeatProblem::GetSource(const Point2D& elementCenter) const {
    return source(elementCenter.x, elementCenter.y, 0);
}

const Mesh &HeatProblem::GetMesh() const { return mesh; }

void HeatProblem::AddBoundaryCondition(const BoundaryCondition &condition) {
    if (hasBoundaryCondition(condition.marker)) {
        throw std::invalid_argument(
            "HeatProblem: boundary condition already exists");
    }
    boundaryConditions.push_back(condition);
}

const BoundaryCondition &HeatProblem::GetBoundaryCondition(int marker) const {
    for (const BoundaryCondition &bc : boundaryConditions) {
        if (bc.marker == marker) {
            return bc;
        }
    }
    throw std::out_of_range("HeatProblem: boundary condition not found");
}

bool HeatProblem::hasBoundaryCondition(int marker) const {
    for (const BoundaryCondition &bc : boundaryConditions) {
        if (bc.marker == marker)
            return true;
    }
    return false;
}

bool HeatProblem::meshHasBoundaryMarker(int marker) {
    return mesh.HasBoundaryMarker(marker);
}

int HeatProblem::BC_size() const { return boundaryConditions.size(); }

BoundaryType HeatProblem::GetTypeBC(int i) const {
    return boundaryConditions[i].type;
}

int HeatProblem::GetMarkerBC(int i) const {
    return boundaryConditions[i].marker;
}

double HeatProblem::GetValue1BC(int i) const {
    return boundaryConditions[i].value1;
}
double HeatProblem::GetValue2BC(int i) const {
    return boundaryConditions[i].value2;
}
