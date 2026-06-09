#include <cstddef>
#include<deque>
#include <iostream>

template<typename T, typename Container = std::deque<T>>
class Queue {
    private:
        Container q;
    public:
        Queue() = default;
        size_t size() const;
        bool empty() const;
        void push(const T& el);
        const T& front() const;
        T& front();
        void pop();        
        bool operator == (const Queue& other) const;
        bool operator != (const Queue& other) const;
        const T& operator() (size_t i) const;
};
template<typename T, typename Container>
std::ostream& operator<<(std::ostream& out, const Queue<T, Container>& q){
    for(size_t i = 0; i < q.size(); i++){
        out<<q(i)<<" ";
    }
    out<<"\n";
    return out;
}

template<typename T, typename Container>
bool Queue<T, Container>::operator!=(const Queue& other) const {
    return !(*this == other);
}

template<typename T, typename Container>
bool Queue<T, Container>::operator==(const Queue& other) const {
    if(this->size() != other.size()) return false;

    for(size_t i = 0; i < this->size(); i++ ){
        if (!(this->q[i] == other(i))) {
            return false;
        }
    }
    return true;
}
template<typename T, typename Container>
const T& Queue<T, Container>::operator()(size_t i) const{
    return q[i];
}
template<typename T, typename Container>
void Queue<T, Container>::pop(){
    q.pop_front();
}

template<typename T, typename Container>
void Queue<T, Container>::push(const T& el) {
    q.push_back(el);
}

template <typename T, typename Container>
const T& Queue<T, Container>::front() const {
    return q.front();
}

template <typename T, typename Container>
T& Queue<T, Container>::front() {
    return q.front();
}


template<typename T, typename Container>
size_t Queue<T, Container>:: size() const{
    return q.size();
}

template<typename T, typename Container> 
bool Queue<T, Container>:: empty() const {
    return q.empty();
}




int main(){
    Queue<int> q1;
    Queue<double> q2;
    q1.push(1);
    q1.push(1);

    q2.push(1);
    q2.push(2.0);

    std::cout<<q1;
    std::cout<<q2;
    std::cout<<(q1 == q2);
    return 0;
}