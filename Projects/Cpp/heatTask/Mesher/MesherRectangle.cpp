#include "MesherRectangle.h"

MesherRectangle::MesherRectangle(int nx_, int ny_) : nx(nx_), ny(ny_) {
    if (nx < 2 || ny < 2) {
        throw std::invalid_argument("MesherRectangle: nx and ny must be >= 2");
    }
    std::cout << "MesherRectangle: MesherRectangle\n";
}

int MesherRectangle::getNodeId(int i, int j) { return nx * i + j; }

Mesh MesherRectangle::generateMesh(const Rectangle &r) {
    std::cout << "MesherRectangle: generateMesh\n";

    double h = r.Height(), w = r.Width();
    double dx = w / (nx - 1), dy = h / (ny - 1);

    Mesh mesh;
    Point2D origin = r.Origin();

    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            mesh.AddNode( {.position = Point2D{dx * j + origin.x, dy * i+ origin.y}});
        }
    }


    for (int i = 0; i < ny - 1; i++) {
        for (int j = 0; j < nx - 1; j++) {
            int n0 = getNodeId(i, j);
            int n1 = getNodeId(i, j + 1);
            int n2 = getNodeId(i + 1, j);
            int n3 = getNodeId(i + 1, j + 1);
            Element el;
            el.nodes_id[0] = n0;
            el.nodes_id[1] = n3;
            el.nodes_id[2] = n2;
            mesh.AddElement(el);
            
            el.nodes_id[0] = n0;
            el.nodes_id[1] = n1;
            el.nodes_id[2] = n3;

            mesh.AddElement(el);;
            
        }
    }
    // std::cout<<"MesherRectangle: mesh.elements.size() =
    // "<<mesh.elements.size()<<std::endl;
    generateBoundaryEdges(mesh);

    return mesh;
}

void MesherRectangle::generateBoundaryEdges(Mesh &m) {
    auto addBoundaryEdge = [&m](int node0, int node1, int marker) {
        m.AddBoundaryEdge({{node0, node1}, marker});
    };

    for (int j = 0; j < nx - 1; ++j) {
        addBoundaryEdge(getNodeId(0, j), getNodeId(0, j + 1),
                        static_cast<int>(RectangleBoundary::Bottom));
    }

    for (int i = 0; i < ny - 1; ++i) {
        addBoundaryEdge(getNodeId(i, nx - 1), getNodeId(i + 1, nx - 1),
                        static_cast<int>(RectangleBoundary::Right));
    }

    for (int j = nx - 1; j > 0; --j) {
        addBoundaryEdge(getNodeId(ny - 1, j), getNodeId(ny - 1, j - 1),
                        static_cast<int>(RectangleBoundary::Top));
    }

    for (int i = ny - 1; i > 0; --i) {
        addBoundaryEdge(getNodeId(i, 0), getNodeId(i - 1, 0),
                        static_cast<int>(RectangleBoundary::Left));
    }
}

