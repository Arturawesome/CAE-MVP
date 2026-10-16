#include "HeatAssembler2D.h"



HeatAssembler2D::HeatAssembler2D(const HeatProblem &problem, const Mesh &mesh)
    : problem_(problem), mesh_(mesh) {}


LinearSystem HeatAssembler2D::Assemble() {
    int numEl = mesh_.NumEl();
    for (int elId = 0; elId < numEl; ++elId) {
        TriangleP1 triangle(mesh_, elId);
        MakeLocalF(triangle);
        MakeLocalK(triangle);
        AddToGlobal(elId);
    }

    return LinearSystem{};
}

void HeatAssembler2D::AddToGlobal(std::size_t elId){

    for (std::size_t i = 0; i < 3; ++i) {
        std::size_t node_i_id = mesh_.NodeIdFromEl(elId, i);

        for (std::size_t j = 0; j < 3; ++j) {
            std::size_t node_j_id = mesh_.NodeIdFromEl(elId, j);
            globK_(node_i_id, node_j_id) += locK_(i, j);
        }

        globF_(node_i_id) += locF_(i);
    }
}

void HeatAssembler2D::MakeLocalF(const TriangleP1& triangle) {
    for (std::size_t i = 0; i < 3; ++i) {
        locF_(i) = problem_.GetSource(triangle.Center()) / 3 * triangle.Area();
    }
}


void HeatAssembler2D::MakeLocalK(const TriangleP1& triangle) {
    double lambda = problem_.GetLambda();    
    locK_ = lambda * triangle.Area() * triangle.Grad().transpose() * triangle.Grad();
}
