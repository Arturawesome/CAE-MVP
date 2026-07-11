#include "MesherRectangle.h"

MesherRectangle::MesherRectangle(int nx_, int ny_) : nx(nx_), ny(ny_)
{
    if (nx < 2 || ny < 2)
    {
        throw std::invalid_argument(
            "MesherRectangle: nx and ny must be >= 2");
    }
    std::cout << "MesherRectangle: MesherRectangle\n";
}

int MesherRectangle::getNodeId(int i, int j)
{
    return nx * i + j;
}

Mesh MesherRectangle::generateMesh(const Rectangle &r)
{
    std::cout << "MesherRectangle: generateMesh\n";

    double h = r.getHeight(), w = r.getWidth();
    double dx = w / (nx - 1), dy = h / (ny - 1);

    Mesh mesh;

    mesh.nodes = std::vector<Node>(nx * ny);
    mesh.elements = std::vector<Element>((ny - 1) * (nx - 1) * 2);
    mesh.boundaryEdges.reserve(2 * (nx - 1) + 2 * (ny - 1));

    for (int i = 0; i < ny; i++)
    {
        for (int j = 0; j < nx; j++)
        {
            int nodeId = getNodeId(i, j);
            mesh.nodes[nodeId] = {nodeId, dx * j, dy * i};
        }
    }

    int element_count = 0;
    for (int i = 0; i < ny - 1; i++)
    {
        for (int j = 0; j < nx - 1; j++)
        {
            int n0 = getNodeId(i, j);
            int n1 = getNodeId(i, j + 1);
            int n2 = getNodeId(i + 1, j);
            int n3 = getNodeId(i + 1, j + 1);
            createElement(n0, n3, n2, element_count, dx, dy,
                          mesh.elements[element_count]);
            ++element_count;

            createElement(n0, n1, n3, element_count, dx, dy,
                          mesh.elements[element_count]);
            ++element_count;
        }
    }
    // std::cout<<"MesherRectangle: mesh.elements.size() = "<<mesh.elements.size()<<std::endl;
    generateBoundaryEdges(mesh);

    return mesh;
}

void MesherRectangle::generateBoundaryEdges(Mesh &m)
{
    auto addBoundaryEdge = [&m](int node0, int node1, int marker)
    {
        m.boundaryEdges.push_back({{node0, node1},
                                   marker});
    };

    for (int j = 0; j < nx - 1; ++j)
    {
        addBoundaryEdge(
            getNodeId(0, j),
            getNodeId(0, j + 1),
            static_cast<int>(RectangleBoundaryMarker::Bottom));
    }

    for (int i = 0; i < ny - 1; ++i)
    {
        addBoundaryEdge(
            getNodeId(i, nx - 1),
            getNodeId(i + 1, nx - 1),
            static_cast<int>(RectangleBoundaryMarker::Right));
    }

    for (int j = nx - 1; j > 0; --j)
    {
        addBoundaryEdge(
            getNodeId(ny - 1, j),
            getNodeId(ny - 1, j - 1),
            static_cast<int>(RectangleBoundaryMarker::Top));
    }

    for (int i = ny - 1; i > 0; --i)
    {
        addBoundaryEdge(
            getNodeId(i, 0),
            getNodeId(i - 1, 0),
            static_cast<int>(RectangleBoundaryMarker::Left));
    }
}

void MesherRectangle::createElement(int n0, int n1, int n2, int element_count,
                                    double dx, double dy, Element &el)
{
    el.nodes_id[0] = n0;
    el.nodes_id[1] = n1;
    el.nodes_id[2] = n2;
    el.s = dx * dy / 2;
    el.id = element_count;
}
