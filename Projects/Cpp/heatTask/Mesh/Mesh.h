#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <utility>

struct Node
{
    int id;
    double x;
    double y;
};

// triangle element: id, area and nodes
struct Element
{
    int id;
    double s;
    std::vector<int> nodes_id = std::vector<int>(3, 0);
};

// marker is !!!NOT THE BOUND CONDIONOS!!!
// marker is the marker of are: left boundary EDGE, right boundary EDGE
struct BoundaryEdge
{
    std::pair<int, int> nodes_id;
    int marker;
};

class Mesh
{
public:
    std::vector<Node> nodes;
    std::vector<Element> elements;
    std::vector<BoundaryEdge> boundaryEdges;
    void saveToSTL(const std::string &filename) const;
    void saveToVTK(const std::string &filename) const;
    bool hasBoundaryMarker(int marker) const;
};