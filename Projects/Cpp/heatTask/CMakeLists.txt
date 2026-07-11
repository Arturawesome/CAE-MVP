cmake_minimum_required(VERSION 4.1.0)
project(HeatTask LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(heatTask 
                main.cpp 
                Mesher/MesherRectangle.cpp
                Mesh/Mesh.cpp
                HeatProblem/HeatProblem.cpp
                SetterBoundaryCondition/SetterBoundaryCondition.cpp
                )



target_include_directories(heatTask PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(heatTask PRIVATE
    -Wall
    -Wextra
    -Werror
)