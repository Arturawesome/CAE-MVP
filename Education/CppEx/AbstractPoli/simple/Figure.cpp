
#include <numeric>
#include <vector>
#include <iostream>

class Figure {
    protected:
        int ma = 0;
        int mb = 0;
        int mc = 0;
    public:
        Figure(int a, int b, int c = 0):ma(a), mb(b), mc(c) {};
        virtual ~Figure() = default;
        virtual int Perimeter() const =0;
};
class Triangle: public Figure{
    public:
        Triangle(int a, int b, int c): Figure(a, b, c) {};
        ~Triangle() override;
        int Perimeter() const override;
};

Triangle::~Triangle() = default;
int Triangle::Perimeter() const {
    return ma + mb + mc;
}


class Rectangle: public Figure{
    public:
        Rectangle(int a, int b): Figure(a, b ) {};
        ~Rectangle() override;
        int Perimeter() const override;
};

Rectangle::~Rectangle() = default;
int Rectangle::Perimeter() const {
    return 2*ma + 2*mb ;
}
