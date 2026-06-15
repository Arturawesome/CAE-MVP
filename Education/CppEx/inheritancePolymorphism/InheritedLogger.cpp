#include"../lifeCycle/LOGGER.cpp"

class InheritedLogger: public Logger {
    public:
        InheritedLogger() {
            std::cout << "InheritedLogger()\n";
        }

        ~InheritedLogger() {
            std::cout << "~InheritedLogger()\n";
        }
};

int main(){
    InheritedLogger x;
}
