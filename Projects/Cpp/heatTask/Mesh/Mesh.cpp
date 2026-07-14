#include "Mesh.h"

#include <fstream>
#include <stdexcept>

std::ostream& operator <<(std::ostream& stream, const Node& node){
    stream<<"{id ="<<node.id<<"; x="<<node.x<<"; y="<<node.y<<"}\n";
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const Element& element){
    stream<<"{id ="<<element.id<<"; s="<<element.s<<";  n_id = ";
    for(const auto node_is: element.nodes_id){
        stream<<node_is<<", ";
    }
    stream<<"}\n ";
    return stream;
}


const Node& Mesh:: GetNode(int id) const {
    return nodes[id];
}
const Element& Mesh:: GetEl(int id) const {
    return elements[id];
}

int Mesh::NumNodesInEl(std::size_t elId) const {
    return elements[elId].nodes_id.size();
}

int Mesh::NodeIdFromEl(int elId, std::size_t i) const {
    if (elId < 0 || static_cast<std::size_t>(elId) >= elements.size())
    {
        throw std::out_of_range(
            "Mesh::NodeIdFromEl: invalid element id");
    }

    if(i >= 0 && i < elements[elId].nodes_id.size()){
        return elements[elId].nodes_id[i];
    }
    throw std::invalid_argument("index of nodes_id is not correct\n");
}

const std::vector<BoundaryEdge>& Mesh:: GetBoundaryEdge() const{
    return boundaryEdges;
}

int  Mesh::GetMarker(int i) const {
    return boundaryEdges[i].marker;
}

int Mesh::NumNodes() const{
    return nodes.size();
}
int Mesh::NumEl() const{
    return elements.size();
}
int Mesh::NumBoundEdges() const{
    return boundaryEdges.size();
}

bool Mesh::hasBoundaryMarker(int marker) const{
    for(const BoundaryEdge& be: boundaryEdges){
        if(be.marker == marker){
            return true;
        }
    }
    return false;
}


void Mesh::saveToVTK(const std::string &filename) const
{
    std::ofstream out(filename);

    if (!out.is_open())
    {
        throw std::runtime_error("Cannot open file " + filename);
    }

    out << "# vtk DataFile Version 3.0\n";
    out << "Triangle mesh\n";
    out << "ASCII\n";
    out << "DATASET UNSTRUCTURED_GRID\n";

    out << "POINTS " << nodes.size() << " double\n";
    for (const auto &node : nodes)
    {
        out << node.x << " " << node.y << " 0.0\n";
    }

    out << "\n";

    out << "CELLS " << elements.size() << " " << elements.size() * 4 << "\n";
    for (const auto &el : elements)
    {
        out << "3 "
            << el.nodes_id[0] << " "
            << el.nodes_id[1] << " "
            << el.nodes_id[2] << "\n";
    }

    out << "\n";

    out << "CELL_TYPES " << elements.size() << "\n";
    for (size_t i = 0; i < elements.size(); ++i)
    {
        out << "5\n";
    }
}

void Mesh::saveToSTL(const std::string &filename) const
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        throw std::runtime_error("Cannot open file " + filename);
    }
    out << "solid mesh\n";

    for (const auto &el : elements)
    {
        const Node &a = nodes[el.nodes_id[0]];
        const Node &b = nodes[el.nodes_id[1]];
        const Node &c = nodes[el.nodes_id[2]];

        out << "  facet normal 0 0 1\n";
        out << "    outer loop\n";

        out << "      vertex " << a.x << " " << a.y << " 0\n";
        out << "      vertex " << b.x << " " << b.y << " 0\n";
        out << "      vertex " << c.x << " " << c.y << " 0\n";

        out << "    endloop\n";
        out << "  endfacet\n";
    }

    out << "endsolid mesh\n";
}