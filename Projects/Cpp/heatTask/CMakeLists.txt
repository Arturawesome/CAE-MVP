cmake_minimum_required(VERSION 3.20)

project(HeatTask LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED)

add_executable(heatTask
    main.cpp
    Mesh/Mesh2D.cpp
    Mesher/MesherRectangle.cpp
    #Mesher/MesherCircle.cpp
    #HeatProblem/HeatProblem.cpp
    #SetterBoundaryCondition/SetterBoundaryCondition.cpp
    #FEMSolver/FEMSolver.cpp
    
)

target_include_directories(heatTask PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(heatTask PRIVATE
    Eigen3::Eigen
)

target_compile_options(heatTask PRIVATE
    -Wall
    -Wextra
    -Werror
)

