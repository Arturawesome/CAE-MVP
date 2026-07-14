#include "FEMSolver.h"
#include <fstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include "../BoundaryCondition/BoundaryConditions.h"
// #include "s21_matrix_oop.h"

FEMSolver::FEMSolver(HeatProblem &problem_) : problem(problem_), mesh(problem_.GetMesh())
{
    std::cout << "FEMSolver::FEMSolver\n";
    int numNodes = mesh.NumNodes();

    glob_F = Eigen::VectorXd::Zero(numNodes);
    solution = Eigen::VectorXd::Zero(numNodes);
    glob_K = Eigen::MatrixXd::Zero(numNodes, numNodes);
}

void FEMSolver::solve()
{
    makeGlobalMatrix();

    addBoundCondToGlobalMat();

    solution = glob_K.partialPivLu().solve(glob_F);

    if (!solution.allFinite())
    {
        throw std::runtime_error(
            "FEMSolver: solution contains NaN or Inf");
    }
    std::cout << solution;
}

void FEMSolver::makeGlobalMatrix()
{
    int numEl = mesh.NumEl();
    for (int elId = 0; elId < numEl; ++elId)
    {
        calcTriangleParamters(elId);
        makeGradMatrix(elId);
        makeLocalF(elId);
        makeLocalK(elId);
        addInGlobalKF(elId);
    }
}

void FEMSolver::addBoundCondToGlobalMat()
{
    int numBC = problem.BC_size();
    for (int i = 0; i < numBC; ++i)
    {
        BoundaryType type = problem.GetTypeBC(i);
        int marker = problem.GetMarkerBC(i);
        double value1 = problem.GetValue1BC(i);
        double value2 = problem.GetValue2BC(i);
        switch (type)
        {
        case BoundaryType::Dirichlet:
            addDirichletToGlobalMatrix(marker, value1);
            break;
        case BoundaryType::Neumann:
            addNeumannToGlobalMatrix(marker, value1, value2);
            break;
        case BoundaryType::Robin:
            break;
        }
    }
    std::cout << "FEMSolver::problem.BC_size() = " << problem.BC_size() << "\n";
}

void FEMSolver::addNeumannToGlobalMatrix(int marker, double value1, double value2)
{
    std::cout << "addNeumannToGlobalMatrix:" << marker << " " << value1 << " " << value2 << "\n";
    return;
}

void FEMSolver::addDirichletToGlobalMatrix(int marker, double value)
{
    std::unordered_set<int> nodes;
    for (const BoundaryEdge &be : mesh.GetBoundaryEdge())
    {
        if (be.marker != marker)
        {
            continue;
        }
        nodes.insert(be.nodes_id.first);
        nodes.insert(be.nodes_id.second);
    }
    if (nodes.empty())
    {
        throw std::runtime_error(
            "FEMSolver: no boundary edges with marker " +
            std::to_string(marker));
    }
    for (const int nodeId : nodes)
    {
        applyDirichletToNode(nodeId, value);
    }
}

void FEMSolver::applyDirichletToNode(int nodeId, double value)
{
    const Eigen::Index s = static_cast<Eigen::Index>(nodeId);
    if (s < 0 || s >= glob_K.rows())
    {
        throw std::out_of_range("FEMSolver: invalid Dirichlet node index");
    }

    glob_F -= glob_K.col(s) * value;

    glob_K.row(s).setZero();
    glob_K.col(s).setZero();

    glob_K(s, s) = 1.0;
    glob_F(s) = value;
}

void FEMSolver::addInGlobalKF(std::size_t elId)
{
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    for (std::size_t i = 0; i < nodes_in_element; ++i)
    {
        std::size_t node_i_id = mesh.NodeIdFromEl(elId, i);

        for (std::size_t j = 0; j < nodes_in_element; ++j)
        {
            std::size_t node_j_id = mesh.NodeIdFromEl(elId, j);
            glob_K(node_i_id, node_j_id) += loc_K(i, j);
        }

        glob_F(node_i_id) += loc_F(i);
    }
}

void FEMSolver::makeLocalK(std::size_t elId)
{
    double lambda = problem.GetLambda();
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    loc_K = Eigen::MatrixXd::Zero(nodes_in_element, nodes_in_element);
    loc_K = lambda * element_area * grad.transpose() * grad;
}

void FEMSolver::makeGradMatrix(std::size_t elId)
{
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    grad = Eigen::MatrixXd::Zero(2, nodes_in_element);
    grad = coefs.block(1, 0, 2, nodes_in_element);
}

void FEMSolver::makeLocalF(std::size_t elId)
{
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);
    loc_F = Eigen::VectorXd::Zero(nodes_in_element);
    for (std::size_t i = 0; i < nodes_in_element; ++i)
    {
        loc_F(i) = problem.GetSource(x_middle, y_middle) / 3 * element_area;
    }
}

void FEMSolver::calcTriangleParamters(std::size_t elId)
{
    triangle = Eigen::MatrixXd::Zero(3, 3);
    std::size_t nodes_in_element = mesh.NumNodesInEl(elId);

    x_middle = 0;
    y_middle = 0;
    for (std::size_t i = 0; i < nodes_in_element; ++i)
    {
        std::size_t node_id = mesh.NodeIdFromEl(elId, i);
        const Node &node = mesh.GetNode(node_id);
        triangle(i, 0) = 1;
        triangle(i, 1) = node.x;
        triangle(i, 2) = node.y;

        x_middle += node.x;
        y_middle += node.y;
    }
    x_middle /= static_cast<double>(nodes_in_element);
    y_middle /= static_cast<double>(nodes_in_element);

    std::cout << mesh.GetEl(elId);
    std::cout << 0.5 * triangle.determinant() << "\n";

    element_area = 0.5 * triangle.determinant();
    coefs = triangle.inverse();
}



void FEMSolver::saveSolution(const std::string &filename) const
{
    if (solution.size() != mesh.NumNodes())
    {
        throw std::runtime_error(
            "FEMSolver::saveSolution: solution size does not match number of mesh nodes");
    }

    std::ofstream out(filename);

    if (!out.is_open())
    {
        throw std::runtime_error(
            "FEMSolver::saveSolution: cannot open file " + filename);
    }

    out << "# vtk DataFile Version 3.0\n";
    out << "FEM temperature solution\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";

    // Узлы
    out << "POINTS " << mesh.NumNodes() << " double\n";

    for (std::size_t nodeId = 0;
         nodeId < static_cast<std::size_t>(mesh.NumNodes());
         ++nodeId)
    {
        const Node &node = mesh.GetNode(nodeId);

        out << node.x << " "
            << node.y << " "
            << 0.0 << "\n";
    }

    out << "\n";

    // Треугольные элементы.
    const std::size_t numElements =
        static_cast<std::size_t>(mesh.NumEl());

    out << "CELLS "
        << numElements << " "
        << numElements * 4 << "\n";

    for (std::size_t elementId = 0;
         elementId < numElements;
         ++elementId)
    {
        const std::size_t numNodesInElement =
            mesh.NumNodesInEl(elementId);

        out << numNodesInElement;

        for (std::size_t localNodeId = 0;
             localNodeId < numNodesInElement;
             ++localNodeId)
        {
            out << " "
                << mesh.NodeIdFromEl(elementId, localNodeId);
        }

        out << "\n";
    }

    out << "\n";

    // Для линейных треугольников VTK cell type = 5.
    out << "CELL_TYPES " << numElements << "\n";

    for (std::size_t elementId = 0;
         elementId < numElements;
         ++elementId)
    {
        out << "5\n";
    }

    out << "\n";

    // Температура в узлах.
    out << "POINT_DATA " << mesh.NumNodes() << "\n";
    out << "SCALARS Temperature double 1\n";
    out << "LOOKUP_TABLE default\n";

    for (Eigen::Index nodeId = 0;
         nodeId < solution.size();
         ++nodeId)
    {
        out << solution(nodeId) << "\n";
    }
}