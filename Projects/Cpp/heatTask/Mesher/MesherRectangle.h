#include <iostream>
#include <iomanip>
#include <cmath>
#include "../Geometry/Rectangle.h"
#include "../Mesh/Mesh.h"

class MesherRectangle {
    private:
        int nx = 1; // numbers of NODES
        int ny = 1; // numbers of NODES
    public:
        MesherRectangle(int nx_, int ny_);
        Mesh generateMesh(const Rectangle& r);
        void createElement(int n0, int n1, int n2, int id_el, 
            double dx, double dy, Element & el);
};