cmake_minimum_required(VERSION 3.20)
project(CAE_MVP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED NO_MODULE)

# Common compile flags and include paths.
add_library(cae_options INTERFACE)
target_include_directories(cae_options INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(cae_options INTERFACE Eigen3::Eigen)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(cae_options INTERFACE -Wall -Wextra -Wpedantic)
endif()

# Geometry consists of header-only classes.
add_library(cae_geometry INTERFACE)
target_link_libraries(cae_geometry INTERFACE cae_options)

# mehs and generators
add_library(cae_mesh STATIC
    mesh/Mesh2D.cpp
    mesh/generators/MesherRectangle.cpp
    
)
target_link_libraries(cae_mesh PUBLIC cae_geometry)

# FEM basis: elements and dofs.
add_library(cae_fem STATIC
    fem/elements/TriangleP1.cpp
    fem/elements/TriangleP2.cpp
    fem/dofs/P2DofMap.cpp
)
target_link_libraries(cae_fem PUBLIC cae_mesh)

# VTK I/O and frame-series helpers.
add_library(cae_io STATIC
    io/VTKReader.cpp
    io/VTKHydroWriter.cpp
    io/VTKSeriesWriter.cpp
)
target_link_libraries(cae_io PUBLIC cae_fem)

# heat equation.
add_library(cae_heat STATIC
    physics/heat/problem/Problem.cpp
    physics/heat/assembly/Assembler2D.cpp
    physics/heat/assembly/BoundaryAssembler2D.cpp
    physics/heat/HeatSolver.cpp
)
target_link_libraries(cae_heat PUBLIC cae_fem)



# Navier-Stokes.
add_library(cae_hydro STATIC
    physics/hydro/problem/HydroProblem.cpp
    physics/hydro/assembly/HydroAssembler.cpp
    physics/hydro/assembly/HydroBoundaryAssembler.cpp
    physics/hydro/HydroSolver.cpp
)
target_link_libraries(cae_hydro PUBLIC cae_fem cae_io)

# Executable.
add_executable(cae_heat_app mainHeat.cpp)
target_link_libraries(cae_heat_app PRIVATE cae_heat cae_io)
set_target_properties(cae_heat_app PROPERTIES OUTPUT_NAME cae_heat)

add_executable(cae_hydro_app mainHydro.cpp)
target_link_libraries(cae_hydro_app PRIVATE cae_hydro cae_heat)
set_target_properties(cae_hydro_app PROPERTIES OUTPUT_NAME cae_hydro)

