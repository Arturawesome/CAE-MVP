#include <cstddef>
#include <vector>
#include <stdexcept>
#include <iostream>
template <typename T>
class Deque {
private:
    std::vector<T> head, tail;

public:
    bool Empty() const;
    size_t Size() const;
    void Clear();
    const T& operator [] (size_t i) const;
    T& operator [] (size_t i);
    void PushFront(const T& elem);
    void PushBack(const T& elem);
    const T& At(size_t i) const;  // throws std::out_of_range on incorrect index
    T& At(size_t i);  // throws std::out_of_range on incorrect index

    const T& Front() const;
    T& Front();
    const T& Back() const;
    T& Back();
    friend std::ostream& operator <<(std::ostream& out, const Deque& deq) {
        for (size_t i = deq.head.size(); i > 0; --i) {
            out << deq.head[i - 1] << " ";
        }
        for (size_t i = 0; i < deq.tail.size(); ++i) {
            out << deq.tail[i] << " ";
        }
        out << "\n";
        return out;
    }
};


template<typename T>
T& Deque<T>:: Back() {
    return  tail.empty() ? head.front() : tail.back(); 
}

template<typename T>
const T& Deque<T>:: Back() const{
    return  tail.empty() ? head.front() : tail.back(); 
}

template<typename T>
T& Deque<T>:: Front() {
    return head.empty() ? tail.front() : head[head.size()-1];
}

template<typename T>
const T& Deque<T>:: Front() const{
    return  head.empty() ? tail.front() : head[head.size()-1];
}

template<typename T>
const T& Deque<T>::At(size_t i)const{
    size_t hs = head.size();
    size_t ts = tail.size();
    size_t s = hs+ ts;
    if(i >= s){
        throw std::out_of_range("on incorrect index");
    } 
    return (*this)[i];
}


template<typename T>
T& Deque<T>::At(size_t i){
    size_t hs = head.size();
    size_t ts = tail.size();
    size_t s = hs+ ts;
    if(i >= s){
        throw std::out_of_range("on incorrect index");
    }
    return (*this)[i];
}

template<typename T>
void Deque<T>:: PushBack(const T& elem){
    tail.push_back(elem);
}

template<typename T>
void Deque<T>:: PushFront(const T& elem){
    head.push_back(elem);
}

template<typename T>
T& Deque<T>::operator [] (size_t i) {
    size_t hs = head.size();
    if (i >= hs){
        return tail[i - hs];
    } else {
        return head[hs-1-i];
    }
}

template<typename T>
const T& Deque<T>::operator [] (size_t i) const{
    size_t hs = head.size();

    if (i >= hs){
        return tail[i - hs];
    } else {
        return head[hs-1-i];
    }
}

template <typename T>
void Deque<T>::Clear(){
    head.clear();
    tail.clear();
}

template<typename T>
size_t Deque<T>::Size() const{
    return head.size() + tail.size();
}

template<typename T>
bool Deque<T>::Empty() const {
    return (head.size() + tail.size()) == 0;
}



int main() {
    Deque<int> d1;
    d1.PushBack(1);
    d1.PushBack(2);
    d1.PushBack(3);
    d1.PushFront(1);
    d1.PushFront(2);
    d1.PushFront(3);
    std::cout<<d1;
    std::cout<<d1.Empty();
    return 0;
}