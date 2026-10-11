#include "VTKReader.h"
#include <iostream>
#include <stdexcept>

Mesh VTKReader::ReadUstructuredGrid(const std::string &filename) {
    std::ifstream in(filename);
    try {
        CheckHeaderUstructuredGrid(in, filename);
    }
    catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        throw e;
    }
    Mesh mesh;
    std::string token;
    std::vector<std::vector<int>> cells;
    std::vector<int> cellTypes;

    bool hasPoints = false;
    bool hasCells = false;
    bool hasTypes = false;

    while (in >> token) {
        if (token == "POINTS") {
            if (hasPoints) {
                throw std::runtime_error("Duplicate POINTS section");
            }
            hasPoints = true;
            try {
                ProcessPoints(in, mesh);
            }
            catch (const std::runtime_error &e) {
                std::cerr << e.what() << std::endl;
                throw e;
            }
        }
        else if (token == "CELLS") {
            if (hasCells) {
                throw std::runtime_error("Duplicate CELLS section");
            }
            hasCells = true;
            try {
                ProcessCells(in, cells, mesh);
            }
            catch (const std::runtime_error &e) {
                std::cerr << e.what() << std::endl;
                throw e;
            }
        }
        else if (token == "CELL_TYPES") {
            if (hasTypes) {
                throw std::runtime_error("Duplicate CELL_TYPES section");
            }
            hasTypes = true;
            try {
                ProcessCellsTypes(in, cellTypes, mesh);
            }
            catch (const std::runtime_error &e) {
                std::cerr << e.what() << std::endl;
                throw e;
            }
        }
        else {
            throw std::runtime_error("Unsupported VTK section: " + token);
        }
    }

    if (!hasPoints || !hasCells || !hasTypes ||
        cells.size() != cellTypes.size()) {
        throw std::runtime_error("Incomplete VTK mesh");
    }

    for (std::size_t i = 0; i < cells.size(); ++i) {
        // VTK_TRIANGLE = 5
        if (cellTypes[i] != 5 || cells[i].size() != 3) {
            throw std::runtime_error("Only triangular cells are supported");
        }

        for (int id : cells[i]) {
            if (id < 0 || id >= mesh.NumNodes()) {
                throw std::runtime_error("Invalid node index");
            }
        }

        mesh.AddElement(Element{{cells[i][0], cells[i][1], cells[i][2]}});
    }
    return mesh;
}
void VTKReader::ProcessCellsTypes(std::istream &in, std::vector<int> &cellTypes,
                                  Mesh &mesh) {
    int count;
    if (!(in >> count) || count < 0) {
        throw std::runtime_error("Invalid CELL_TYPES header");
    }

    for (int i = 0; i < count; ++i) {
        int type;
        if (!(in >> type)) {
            throw std::runtime_error("Invalid cell type");
        }
        cellTypes.push_back(type);
    }
}

void VTKReader::ProcessCells(std::istream &in,
                             std::vector<std::vector<int>> &cells, Mesh &mesh) {
    int count, totalSize;
    if (!(in >> count >> totalSize) || count < 0 || totalSize < 0) {
        throw std::runtime_error("Invalid CELLS header");
    }

    int consumed = 0;

    for (int i = 0; i < count; ++i) {
        int n;
        if (!(in >> n) || n < 0 || n > totalSize) {
            throw std::runtime_error("Invalid cell size");
        }

        consumed += n + 1;
        std::vector<int> ids;

        for (int j = 0; j < n; ++j) {
            int id;
            if (!(in >> id)) {
                throw std::runtime_error("Invalid cell index");
            }
            ids.push_back(id);
        }

        cells.push_back(std::move(ids));
    }

    if (consumed != totalSize) {
        throw std::runtime_error("Invalid CELLS total size");
    }
}

void VTKReader::ProcessPoints(std::istream &in, Mesh &mesh) {
    int N = 0;
    std::string dataType;
    if (!(in >> N >> dataType)) {
        throw std::runtime_error("Invalid POINTS header");
    }
    for (int i = 0; i < N; ++i) {
        double x, y, z;
        if (!(in >> x >> y >> z)) {
            throw std::runtime_error("Invalid POINTS header");
        }
        if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(z) ||
            std::abs(z) > 1e-10) {
            throw std::runtime_error("Mesh must lie in the XY plane");
        }
        mesh.AddNode(Node{x, y});
    }
}

void VTKReader::CheckHeaderUstructuredGrid(std::istream &in,
                                           const std::string &filename) {
    if (!in) {
        throw std::runtime_error("Cannot open VTK: " + filename);
    }
    std::string line;
    std::getline(in, line); /*# vtk DataFile Version 3.0 */
    if (line.rfind("vtk") != 0) {
        throw std::runtime_error("Invalid vtk header");
    }
    std::getline(in, line); // smth
    std::getline(in, line); // ASKII
    if (line != "ASKII") {
        throw std::runtime_error("Only ASCII VTK is supported");
    }
    std::string token;
    in >> token;
    if (token != "DATASET") {
        throw std::runtime_error("Expected DATASET");
    }

    in >> token;
    if (token != "UNSTRUCTURED_GRID") {
        throw std::runtime_error("Only UNSTRUCTURED_GRID is supported");
    }
}