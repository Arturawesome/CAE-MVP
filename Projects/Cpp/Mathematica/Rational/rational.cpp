#include <numeric>
#include<iostream>

class Rational {
    private:
        int num = 0;
        int denom = 1;

    public:
        Rational() = default;
        Rational(int n, int d);
        Rational(const Rational& other);

        int GetNum() const;
        int GetDenom() const;
        int Numerator() const;
        int Denominator() const;

        Rational& operator += (const Rational& other);
        Rational& operator -= (const Rational& other);
        Rational& operator *= (const Rational& other);
        Rational& operator /= (const Rational& other);
        

        Rational& operator += (int a);
        Rational& operator -= (int a);
        Rational& operator *= (int a);
        Rational& operator /= (int a);

        Rational operator -() const ;
        Rational operator +() const ;

        Rational operator -(const Rational& other) const;
        Rational operator +(const Rational& other) const;
        Rational operator*(const Rational& other) const;
        Rational operator/(const Rational& other) const;

        Rational operator*(int a) const;
        Rational operator/(int a) const;
        Rational operator+(int a) const;
        Rational operator-(int a) const;

        bool operator==(const Rational& other) const;
        bool operator!=(const Rational& other) const;
        bool operator==(int other) const;
        bool operator!=(int other) const;

};
Rational Rational:: operator -(const Rational& other)const{
    Rational res(*this);
    return res -=other;
}

Rational Rational:: operator +(const Rational& other)const{
    Rational res(*this);
    return res +=other;
}

Rational Rational:: operator -() const{
    return Rational(-this->num, this->denom);
}
Rational Rational:: operator +() const{
    return Rational(this->num, this->denom);
}
bool Rational::operator==(const Rational& other)const{
    if(this->Numerator() == other.Numerator() && this->Denominator() == other.Denominator()){
        return true;
    }
    return false;
}
bool Rational::operator!=(const Rational& other)const{
    return !(*this == other);
}
bool Rational::operator==(int other)const{
    Rational r1(other, 1);
    return *this == r1;
}
bool Rational::operator!=(int other)const{
    Rational r1(other, 1);
    return *this != r1;
}

Rational& Rational::operator += (int a){
    Rational r1(a, 1);
    return *this +=r1;
}
Rational& Rational::operator -= (int a){
    Rational r1(a, 1);
    return *this -= r1;
}
Rational& Rational::operator *= (int a){
    Rational r1(a, 1);
    return *this *= r1;
}
Rational& Rational::operator /= (int a){
    Rational r1(a, 1);
    return *this /= r1;
}


Rational& Rational:: operator *= (const Rational& other){
    this->num *= other.GetNum();
    this->denom *= other.GetDenom();
    return *this;
}

Rational& Rational:: operator /= (const Rational& other){
    this->num *= other.GetDenom();
    this->denom *= other.GetNum();
    return *this;
}

Rational& Rational:: operator += (const Rational& other){
    int genDenom = std::abs(this->denom * other.GetDenom()) / std::gcd(this->denom, other.GetDenom());
    int newNumThis = this->num * genDenom / this->denom;
    int newNumOther = other.GetNum() * genDenom / other.GetDenom();
    this->num = newNumThis + newNumOther;
    this -> denom = genDenom;
    return *this;
}
Rational& Rational:: operator -= (const Rational& other){
     int genDenom = std::abs(this->denom * other.GetDenom()) / std::gcd(this->denom, other.GetDenom());
    int newNumThis = this->num * genDenom / this->denom;
    int newNumOther = other.GetNum() * genDenom / other.GetDenom();
    this->num = newNumThis - newNumOther;
    this -> denom = genDenom;
    return *this;
}

Rational::Rational(const Rational& other): num(other.GetNum()), denom(other.GetDenom()) {}

Rational::Rational(int n, int d) {
    num = 0;
    denom = 1;
    if (d == 0) return;
    num = n;
    denom = d;
    if (denom < 0) {
        num = -num;
        denom = -denom;
    }
}

int Rational:: GetDenom() const {
    return denom;
}
int Rational:: GetNum() const {
    return num;
}

int Rational::Numerator() const{
    int d = std::gcd(num, denom);
    return num / d;
}
int Rational::Denominator() const{
    int d = std::gcd(num, denom);
    return denom / d;
}

int main() {
    Rational r(3, 10);
    Rational r2(-5, 8);
    std::cout<<r.Numerator();
    std::cout<<r.Denominator();
    std::cout<<"\n";
    r += r2;
    
    std::cout<<r.GetNum()<<"/";
    std::cout<<r.GetDenom();
}