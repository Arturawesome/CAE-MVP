#include "FEMSolver.h"
#include "../BoundaryCondition/BoundaryConditions.h"
#include "elements/TriangleP1.h"
#include "assembly/HeatAssembler2D.h"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>


FEMSolver::FEMSolver(HeatProblem &problem_)
    : problem(problem_), mesh(problem_.GetMesh()) {
    std::cout << "FEMSolver::FEMSolver\n";
    int numNodes = mesh.NumNodes();

    glob_F = Eigen::VectorXd::Zero(numNodes);
    solution = Eigen::VectorXd::Zero(numNodes);
    glob_K = Eigen::MatrixXd::Zero(numNodes, numNodes);

    
}

void FEMSolver::Solve() {
    HeatAssembler2D assembler(problem, mesh);
    glob_K.setZero();
    glob_F.setZero();
    solution.setZero();
    

    makeGlobalMatrix();

    addBoundCondToGlobalMat();

    solution = glob_K.partialPivLu().solve(glob_F);

    if (!solution.allFinite()) {
        throw std::runtime_error("FEMSolver: solution Contains NaN or Inf");
    }
    
}

void FEMSolver::makeGlobalMatrix() {
    int numEl = mesh.NumEl();
    for (int elId = 0; elId < numEl; ++elId) {

        TriangleP1 triangle(mesh, elId);
        triangle.Area();
        triangle.Center();
        triangle.Grad();
        calcTriangleParamters(elId);
        makeGradMatrix(elId);


        makeLocalF(elId);
        makeLocalK(elId);
        addInGlobalKF(elId);
    }
}

void FEMSolver::addBoundCondToGlobalMat() {
    int numBC = problem.BC_size();
    for (int i = 0; i < numBC; ++i) {
        BoundaryType type = problem.GetTypeBC(i);
        int marker = problem.GetMarkerBC(i);
        double value1 = problem.GetValue1BC(i);
        double value2 = problem.GetValue2BC(i);
        switch (type) {
        case BoundaryType::Neumann:
            addNeumannToGlobalMatrix(marker, value1);
            break;
        case BoundaryType::Robin:
            addRobinToGlobalMatrix(marker, value1, value2);
            break;
        case BoundaryType::Dirichlet:
            addDirichletToGlobalMatrix(marker, value1);
            break;
        }
    }
    
}

void FEMSolver::addRobinToGlobalMatrix(int marker, double value1,
                                       double value2) {
    std::cout << marker << value1 << value2 << "\n";
}

void FEMSolver::addNeumannToGlobalMatrix(int marker, double flux) {
    std::size_t processedEdges = 0;

    for (const BoundaryEdge &be : mesh.BoundaryEdges()) {
        if (be.boundaryId != marker) {
            continue;
        }
        applyNeumannToEdge(be, flux);
        ++processedEdges;
    }
    if (processedEdges == 0) {
        throw std::runtime_error("FEMSolver: no boundary edges with marker " +
                                 std::to_string(marker));
    }

    return;
}

void FEMSolver::applyNeumannToEdge(const BoundaryEdge &be, double flux) {
    int id1 = be.nodes_id.first;
    int id2 = be.nodes_id.second;

    const Node &node1 = mesh.NodeById(id1);
    const Node &node2 = mesh.NodeById(id2);

    double dx = node2.position.x - node1.position.x;
    double dy = node2.position.y - node1.position.y;

    const double edgeLength = std::hypot(dx, dy);
    if (edgeLength <= 0.0) {
        throw std::runtime_error(
            "FEMSolver::applyNeumannToEdge: zero-length boundary edge");
    }

    double localContribution = flux * 0.5 * edgeLength;

    glob_F[id1] += localContribution;
    glob_F[id2] += localContribution;
}

void FEMSolver::addDirichletToGlobalMatrix(int marker, double value) {
    std::unordered_set<int> nodes;
    for (const BoundaryEdge &be : mesh.BoundaryEdges()) {
        if (be.boundaryId != marker) {
            continue;
        }
        nodes.insert(be.nodes_id.first);
        nodes.insert(be.nodes_id.second);
    }
    if (nodes.empty()) {
        throw std::runtime_error("FEMSolver: no boundary edges with marker " +
                                 std::to_string(marker));
    }
    for (const int nodeId : nodes) {
        applyDirichletToNode(nodeId, value);
    }
}

void FEMSolver::applyDirichletToNode(int nodeId, double value) {
    const Eigen::Index s = static_cast<Eigen::Index>(nodeId);
    if (s < 0 || s >= glob_K.rows()) {
        throw std::out_of_range("FEMSolver: invalid Dirichlet node index");
    }

    glob_F -= glob_K.col(s) * value;

    glob_K.row(s).setZero();
    glob_K.col(s).setZero();

    glob_K(s, s) = 1.0;
    glob_F(s) = value;
}

void FEMSolver::addInGlobalKF(std::size_t elId) {
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    for (std::size_t i = 0; i < nodes_in_element; ++i) {
        std::size_t node_i_id = mesh.NodeIdFromEl(elId, i);

        for (std::size_t j = 0; j < nodes_in_element; ++j) {
            std::size_t node_j_id = mesh.NodeIdFromEl(elId, j);
            glob_K(node_i_id, node_j_id) += loc_K(i, j);
        }

        glob_F(node_i_id) += loc_F(i);
    }
}

void FEMSolver::makeLocalK(std::size_t elId) {
    double lambda = problem.GetLambda();
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    loc_K = Eigen::MatrixXd::Zero(nodes_in_element, nodes_in_element);
    loc_K = lambda * element_area * grad.transpose() * grad;
}

void FEMSolver::makeGradMatrix(std::size_t elId) {
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    grad = Eigen::MatrixXd::Zero(2, nodes_in_element);
    grad = coefs.block(1, 0, 2, nodes_in_element);
}

void FEMSolver::makeLocalF(std::size_t elId) {
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    loc_F = Eigen::VectorXd::Zero(nodes_in_element);
    for (std::size_t i = 0; i < nodes_in_element; ++i) {
        loc_F(i) = problem.GetSource(x_middle, y_middle) / 3 * element_area;
    }
}

void FEMSolver::calcTriangleParamters(std::size_t elId) {
    triangle = Eigen::MatrixXd::Zero(3, 3);
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);

    x_middle = 0;
    y_middle = 0;
    for (std::size_t i = 0; i < nodes_in_element; ++i) {
        std::size_t node_id = mesh.NodeIdFromEl(elId, i);
        const Node &node = mesh.NodeById(node_id);
        triangle(i, 0) = 1;
        triangle(i, 1) = node.position.x;
        triangle(i, 2) = node.position.y;

        x_middle += node.position.x;
        y_middle += node.position.y;
    }
    x_middle /= static_cast<double>(nodes_in_element);
    y_middle /= static_cast<double>(nodes_in_element);



    element_area = 0.5 * triangle.determinant();
    coefs = triangle.inverse();
}

Eigen::VectorXd& FEMSolver::Solution(){
    return solution;
}

