#pragma once
#include <vector>
#include <fstream>
#include <string>

struct Node {
    int id;
    double x;
    double y;
};

//triangle element
struct Element{
    int id;
    double s;
    std::vector<int> nodes_id = std::vector<int>(3, 0);
    
};

enum class BoundaryType {
    Dirichlet, // 1 род: T = задано
    Neumann,   // 2 род: поток
    Robin      // 3 род: теплообмен
};

struct BoundaryEdge {
    std::vector<int> nodes_id;
    BoundaryType type;
};

class Mesh {
    public:
        std::vector<Node> nodes;
        std::vector<Element> elements;
        std::vector<BoundaryEdge> boundaryEdges;
        void saveToSTL(const std::string& filename) const;
        void saveToVTK(const std::string& filename) const;
};