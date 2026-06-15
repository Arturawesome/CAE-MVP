#include <numeric>
#include<iostream>

class Rational {
    private:
        int num = 0;
        int denom = 1;
        void normalize() {
            int g = std::gcd(num, denom);

            num /= g;
            denom /= g;

            if (denom < 0) {
                num = -num;
                denom = -denom;
            }
        }
    public:
        Rational() = default;
        Rational(int n, int d);
        Rational(const Rational& other);

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
Rational Rational::operator+(int a) const {
    Rational res(*this);
    return res += a;
}

Rational Rational::operator-(int a) const {
    Rational res(*this);
    return res -= a;
}

Rational Rational::operator*(int a) const {
    Rational res(*this);
    return res *= a;
}

Rational Rational::operator/(int a) const {
    Rational res(*this);
 
    return res /= a;
}
Rational Rational:: operator/(const Rational& other) const {
    Rational res(*this);
    return res /= other;
}

Rational Rational:: operator*(const Rational& other) const {
    Rational res(*this);
    return res *= other;
}

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
    this->num *= other.Numerator();
    this->denom *= other.Denominator();
     normalize();
    return *this;
}

Rational& Rational:: operator /= (const Rational& other){
    this->num *= other.Denominator();
    this->denom *= other.Numerator();
    normalize();
    return *this;
}

Rational& Rational:: operator += (const Rational& other){
    int genDenom = std::abs(this->denom * other.Denominator()) / std::gcd(this->denom, other.Denominator());
    int newNumThis = this->num * genDenom / this->denom;
    int newNumOther = other.Numerator() * genDenom / other.Denominator();
    this->num = newNumThis + newNumOther;
    this -> denom = genDenom;
    normalize();
    return *this;
}
Rational& Rational:: operator -= (const Rational& other){
     int genDenom = std::abs(this->denom * other.Denominator()) / std::gcd(this->denom, other.Denominator());
    int newNumThis = this->num * genDenom / this->denom;
    int newNumOther = other.Numerator() * genDenom / other.Denominator();
    this->num = newNumThis - newNumOther;
    this -> denom = genDenom;
    normalize();
    return *this;
}

Rational::Rational(const Rational& other): num(other.Numerator()), denom(other.Denominator()) {}

Rational::Rational(int n, int d) {
    if (d == 0) {
        num = 0;
        denom = 1;
        return;
    }

    num = n;
    denom = d;

    normalize();
}

int Rational::Numerator() const{
    return num ;
}
int Rational::Denominator() const{
    return denom;
}

Rational operator+(int a, Rational& r) {
    return r+a;
}
Rational operator-(int a, Rational& r) {
    return r-a;
}

Rational operator*(int a, Rational& r) {
    return r*a;
}

Rational operator/(int a, Rational& r) {
    return r/a;
}



int main() {
    Rational r(3, 10);
    Rational r2(-5, 8);
    std::cout<<r.Numerator();
    std::cout<<r.Denominator();
    std::cout<<"\n";
    r += r2;
    
    std::cout<<r.Numerator()<<"/";
    std::cout<<r.Denominator();
}