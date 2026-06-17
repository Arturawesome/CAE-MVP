#include "AdvancedVector.cpp"
#include <iostream>
int main (){
    AdvancedVector<int> v1;
    AdvancedVector<double> v2;
    AdvancedVector<double> v3;
    v1.push_back(1);
    v1.push_back(2);
    v2.push_back(1.1);
    v2.push_back(2.2);
    v3.push_back(2.2);
    std::cout<<v1[0]<<" "<<v1[1]<<"\n";
    std::cout<<v2[0]<<" "<<v2[1]<<"\n";
    std::cout<<v1[-1]<<" "<<v1[-2]<<"\n";
    std::cout<<v2[-1]<<" "<<v2[-2]<<"\n";
    std::cout<<(v2 == v3)<<"\n";
    std::cout<<v3[-12]<<"\n";
    std::cout<<v3[123]<<"\n";
    return 0;
}