#include <cstddef>
#include <iostream>
#include <vector>

template <typename T>
class Table {
    private:
        size_t mr = 0, mc = 0;
        std::vector<std::vector<T>> mt;
    public:
        Table(size_t r, size_t c);

        const std::vector<T>& operator[](size_t i) const;
        std::vector<T>& operator[](size_t i);

        void resize(size_t r, size_t c);
        std::pair<size_t, size_t> size() const;
    
};


template <typename T>
std::ostream& operator<< (std::ostream& out, const Table<T>& t){
    std::pair<size_t, size_t> s = t.size();
    for(size_t i = 0; i < s.first; i++){
        for(size_t j = 0; j < s.second; j++){
            out<<t[i][j]<<" ";
        }
        out<<"\n";
    }
    return out;
}


template <typename T>
const std::vector<T>&Table<T>::operator[](size_t i)const {
    return mt[i];
}

template <typename T>
std::vector<T>&Table<T>::operator[](size_t i){
    return mt[i];
}

template <typename T>
Table<T>::Table(size_t r, size_t c): mr(r), mc(c) {
    this->resize(r, c);
}

template <typename T>
void Table<T>:: resize(size_t r, size_t c) {
    mt.resize(r);
    for(auto &row: mt){
        row.resize(c);
    }
    mr = r;
    mc = c;
}

template <typename T>
std::pair<size_t, size_t>Table<T>::  size() const{
    return std::pair<size_t, size_t>(mr, mc);
}


