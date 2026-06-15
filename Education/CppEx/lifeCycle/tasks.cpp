
#include <iostream>
#include <vector>
class Logger {
private:
    static int counter;
    const int id;

public:
    Logger(): id(++counter) {
        std::cout << "Logger(): " << id << "\n";
    }

    Logger(const Logger& other): id(++counter) {
        std::cout << "Logger(const Logger&): " << id << " " << other.id << "\n";
    }

    Logger(Logger&& other): id(++counter) {
        std::cout << "Logger(Logger&&): " << id << " " << other.id << "\n";
    }

    Logger& operator = (const Logger& other) {
        std::cout << "Logger& operator = (const Logger&): " << id << " " << other.id << "\n";
        return *this;
    }

    Logger& operator = (Logger&& other) {
        std::cout << "Logger& operator = (Logger&&): " << id << " " << other.id << "\n";
        return *this;
    }

    ~Logger() {
        std::cout << "~Logger(): " << id << "\n";
    }
};

int Logger::counter = 0;


int main (){
    // task 1 -------- task 1 ---------  task 1
    /*
    Logger(): 1
    ~Logger(): 1
    Logger(): 2
    ~Logger(): 2
    */
    // Logger* lptr1 = new Logger();
    // delete lptr1;
    // Logger* lptr2 = new Logger();
    // delete lptr2;



    // task 2 -------- task 2 ---------  task 2
    /*
    Logger(): 1
    Logger(const Logger&): 2 1
    ~Logger(): 2
    ~Logger(): 1
    */
    // Logger x1;
    // Logger x2(x1);

    // task 3 -------- task 3 ---------  task 3
    /*
    Logger(): 1
    Logger(Logger&&): 2 1
    ~Logger(): 2
    ~Logger(): 1
    */
    // Logger x1;
    // Logger x2 = std::move(x1);

    // task 4 -------- task 4 ---------  task 4
    /*
    Logger(): 1
    Logger(): 2
    Logger& operator = (const Logger&): 1 2
    Logger& operator = (Logger&&): 1 2
    ~Logger(): 2
    ~Logger(): 1
    */
    // Logger x1;
    // Logger x2;
    // x1 = x2;
    // x1 = std::move(x2);

    // task 5 -------- task 5 ---------  task 5
    /*
    Logger(): 1
    Logger(): 2
    Logger(): 3
    ~Logger(): 2
    ~Logger(): 3
    ~Logger(): 1
    */
    // Logger* lptr1 = new Logger();
    // Logger* lptr2 = new Logger();
    // Logger* lptr3 = new Logger();
    // delete lptr2;
    // delete lptr3;
    // delete lptr1;

    int n = 0;
    std::cin >>n;
    std::vector<Logger*>v1;
    v1.reserve(n);
    for(int i = 0; i < n; i++){
        Logger *lptr = new Logger();
        v1.push_back(lptr);
    }
    for(int i = n-1; i >= 0; i--){
        delete v1[i];
    }
    
    return 0;
}