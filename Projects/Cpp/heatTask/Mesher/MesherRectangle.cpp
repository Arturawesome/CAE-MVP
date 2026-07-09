#include "MesherRectangle.h"


MesherRectangle:: MesherRectangle(int nx_, int ny_): nx(nx_), ny(ny_) {
    std::cout<<"MesherRectangle: MesherRectangle\n";
}


Mesh MesherRectangle::generateMesh(const Rectangle& r) {
    std::cout<<"MesherRectangle: generateMesh\n";
    double h = r.getHeight(), w = r.getWidth();
    double dx = w / (nx-1), dy = h / (ny-1);
    Mesh mesh;
    mesh.nodes = std::vector<Node>(nx * ny);
    mesh.boundaryEdges.nodes_id.reserve(int(pow(1.5*nx*ny, 0.5)));
    mesh.elements = std::vector<Element>((ny-1)*(nx-1) * 2);

    for(int i = 0; i < ny; i++ ){
        for(int j = 0; j < nx; j++){
            mesh.nodes[nx*i + j] = {.x = dx * j, .y = dy * i, .id = nx*i + j};
            if(i == 0 || j == 0 || i == ny-1 || j == nx-1){
                mesh.boundaryEdges.nodes_id.push_back(nx*i + j);
                mesh.boundaryEdges.markers.push_back(1);
            }

        }
    }
    std::cout<<"MesherRectangle: mesh.nodes.size() = "<<mesh.nodes.size()<<std::endl;

    auto getNodeId = [this](int i, int j){
        return nx*i + j;
    };

    int element_count = 0;
    for(int i = 0; i < ny-1; i++ ){
        for(int j = 0; j < nx-1; j++){
            int n0 = getNodeId(i, j);
            int n1 = getNodeId(i, j +1);
            int n2 = getNodeId(i+1, j);
            int n3 = getNodeId(i+1, j+1);
            createElement(n0, n1, n2, element_count, dx, dy, 
                mesh.elements[element_count]);
            ++element_count;
            
            createElement(n1, n2, n3, element_count, dx, dy, 
                mesh.elements[element_count]);
            ++element_count;
        }
    }
    std::cout<<"MesherRectangle: mesh.elements.size() = "<<mesh.elements.size()<<std::endl;
    
    return mesh;
}  

void MesherRectangle::createElement(int n0, int n1, int n2, int element_count, 
    double dx, double dy, Element & el){
    el.nodes_id[0] = n0;
    el.nodes_id[1] = n1;
    el.nodes_id[2] = n2;
    el.s = dx*dy/2;
    el.id = element_count;
}











