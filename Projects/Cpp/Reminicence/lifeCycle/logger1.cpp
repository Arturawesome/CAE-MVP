#include<iostream>
//#include<list>
class Logger {
    private:
        const int id;
        inline static int counter = 0;
    public:
        Logger();
        Logger(const Logger& other);
        //move constructor
        Logger(Logger&& other): id(++counter){
            std::cout << "Logger(Logger&&): " << id << " " << other.id << "\n";
        }
        Logger& operator = (Logger&& other) {
           std::cout << "Logger& operator = (Logger&&) " << id << " " << other.id << "\n";
            return *this;
        }
        ~Logger();
        Logger& operator =(const Logger& other);
};

Logger& Logger::operator =(const Logger& other){
    std::cout << "Logger& operator = (const Logger&) " << id << " " << other.id << "\n";
    return *this;
}

Logger::Logger(): id(++counter) {
    std::cout<<"Logger() "<<id<<"\n";
}

Logger::Logger(const Logger& other): id(++counter) {
    std::cout << "Logger(const Logger&): " << id << " " << other.id << "\n";
}
Logger::~Logger(){
    std::cout<<"~Logger()"<<id<<"\n";
}



class OuterLogger {
    private:
        Logger innerLogger1;
        Logger innerLogger2;

        inline static int counter = 0;
        const int id;
 public:
    OuterLogger(): id(++counter) {
        std::cout << "OuterLogger(): " << id << "\n";
    }

    OuterLogger(const OuterLogger& other):
        innerLogger1(other.innerLogger1),  // инициализируем поля
        innerLogger2(other.innerLogger2),  // в порядке их объявления
        id(++counter)
    {
        std::cout << "OuterLogger(const OuterLogger&): " << id << " " << other.id << "\n";
    }

    OuterLogger& operator = (const OuterLogger& other) {
        innerLogger1 = other.innerLogger1;  // вызываем оператор присваивания для полей
        innerLogger2 = other.innerLogger2;
        std::cout << "OuterLogger& operator = (const OuterLogger&) " << id << " " << other.id << "\n";
        return *this;
    }

    ~OuterLogger() {
        std::cout << "~OuterLogger() " << id << "\n";
    }
};

void f(const Logger& x) {
    std::cout << "void f(const Logger&)\n";
}

int main(){
     Logger x1;
    Logger x2 = x1;  // вызывается обычный конструктор копирования
    Logger x3 = Logger();  // сработает copy elision: временный объект даже не будет создаваться
    Logger x4 = std::move(x1);
    return 0;
}