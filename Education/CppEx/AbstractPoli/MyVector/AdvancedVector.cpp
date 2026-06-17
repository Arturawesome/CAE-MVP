
#include<vector>
#include <stdexcept>

template <typename T>
class AdvancedVector: public std::vector<T> {        
    public: 
        using std::vector<T>::vector;

        AdvancedVector() = default;
        
        template<typename inputIt>
        AdvancedVector(inputIt first, inputIt second);

        ~AdvancedVector() = default;
        T& operator[](int i);
        const T& operator[](int i) const;

        bool operator == (const AdvancedVector& other) const;
        bool operator != (const AdvancedVector& other) const;
};


template <typename T>
template<typename inputIt>
AdvancedVector<T>::AdvancedVector(inputIt first, inputIt second): std::vector<T>(first, second){}


template <typename T>
T& AdvancedVector<T>:: operator[](int i) {
    
    if(i >= 0){
        if(size_t(i) >= this->size()){
            throw std::out_of_range("Index out of range");
        }
        return this->std::vector<T>::operator[](size_t(i));
    }
    if(size_t(-i) > this->size()){
         throw std::out_of_range("Index out of range");
    }
    return this->std::vector<T>::operator[](this->size() + i);
 }


template<typename T>
const T& AdvancedVector<T>:: operator [](int i) const {
    if(i >= 0){
        if(size_t(i) >= this->size()){
            throw std::out_of_range("Index out of range");
        }
        return this->std::vector<T>::operator[](size_t(i));
    }
    if(size_t(-i) > this->size()){
         throw std::out_of_range("Index out of range");
    }
    return this->std::vector<T>::operator[](this->size() + i);
}

template<typename T>
bool  AdvancedVector<T>:: operator == (const AdvancedVector& other) const{
    //return this->std::vector<T>::operator == (other);
    return static_cast<const std::vector<T>&>(*this)
            == static_cast<const std::vector<T>&>(other);
}

template<typename T>
bool AdvancedVector<T>:: operator != (const AdvancedVector& other) const{
    //return this->std::vector<T>::operator != (other);
     return !(*this == other);
}