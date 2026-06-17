#include<vector>
#include<iostream>
#include<iterator>

template <typename T>
class AdvancedVector {
    private:
        std::vector<T> v;
    public: 
        AdvancedVector();
        AdvancedVector(const AdvancedVector* other);
        
        template<typename inputIt>
        AdvancedVector(inputIt first, inputIt second);
};