#include<iostream>
#include<list>
class Logger {
    private:
        int id = 0;
    public:
        Logger();
        Logger(int x);
        ~Logger();
};

Logger::Logger(int x): id(x) {
    std::cout<<"Logger() "<<id<<"\n";
}

Logger::Logger(){
    std::cout<<"Logger()"<<id<<"\n";
}
Logger::~Logger(){
    std::cout<<"~Logger()"<<id<<"\n";
}

int main(){
    /*
    // memory in stack
    Logger x;
    Logger x2(1);
    {
        Logger x2(2);
    }
    Logger x3(3);

    // memory in dynamic memory
    Logger* lptr1 = new Logger(11);
    Logger* lptr2 = new Logger(22);
    delete lptr1;
    delete lptr2;

    std::list<Logger> loggers(2);
    loggers.pop_front();
    loggers.pop_back();
    */
    // one constructor but 2 destr =-> default copy constructior woithout print
    //Logger x1(1);
    //Logger x2 = x1;

    // two destructors only for 11 => default operator = wothout prints
    //Logger x11(11);
    //Logger x22(22);
    //x22 = x11;
    std::list<Logger> loggers(2);
    loggers.pop_back();
    loggers.pop_front();

    return 0;
}
