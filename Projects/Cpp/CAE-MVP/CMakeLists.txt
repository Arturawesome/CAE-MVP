cmake_minimum_required(VERSION 3.20)

project(CAE_MVP LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Eigen3 REQUIRED)

add_executable(cae_mvp
    main.cpp
    mesh/Mesh2D.cpp
    mesh/generators/MesherRectangle.cpp
    io/VTKReader.cpp
    
    physics/heat/problem/Problem.cpp
    physics/heat/assembly/Assembler2D.cpp
    physics/heat/assembly/BoundaryAssembler2D.cpp
    physics/heat/HeatSolver.cpp

    fem/elements/TriangleP1.cpp
    fem/systems/LinearSystem.h
    fem/dofs/P2DofMap.cpp


)

target_include_directories(cae_mvp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cae_mvp PRIVATE
    Eigen3::Eigen
)

target_compile_options(cae_mvp PRIVATE
    -Wall
    -Wextra
    -Werror
)

