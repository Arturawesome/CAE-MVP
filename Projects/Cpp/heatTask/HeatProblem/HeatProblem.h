#pragma once
#include <iostream>
class HeatProblem {
    private:
    public:
        // download geometry and set conditions
        HeatProblem(std::string filename, std::unordered_map<std::string, std::string>& physics_conditions);

        // set figure and set conditions
        HeatProblem(std::string figure, int num_elemnts, std::unordered_map<std::string, std::string>& physics_conditions);
        ~HeatProblem() = default;

        void solveProblem();
        
};