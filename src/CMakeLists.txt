cmake_minimum_required(VERSION 3.20)
project(CAE_MVP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED NO_MODULE)

# Common compile flags and include paths.
add_library(cae_options INTERFACE)
target_include_directories(cae_options INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/core")
target_link_libraries(cae_options INTERFACE Eigen3::Eigen)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(cae_options INTERFACE -Wall -Wextra -Wpedantic)
endif()

# Geometry consists of header-only classes.
add_library(cae_geometry INTERFACE)
target_link_libraries(cae_geometry INTERFACE cae_options)

# mehs and generators
add_library(cae_mesh STATIC
core/mesh/Mesh2D.cpp
   core/mesh/generators/MesherRectangle.cpp
)
target_link_libraries(cae_mesh PUBLIC cae_geometry)

# FEM basis: elements and dofs.
add_library(cae_fem STATIC
    core/fem/elements/TriangleP1.cpp
    core/fem/elements/TriangleP2.cpp
    core/fem/dofs/P2DofMap.cpp
)
target_link_libraries(cae_fem PUBLIC cae_mesh)

# VTK I/O and frame-series helpers.
add_library(cae_io STATIC
    core/io/VTKReader.cpp
    core/io/VTKHydroWriter.cpp
    core/io/VTKSeriesWriter.cpp
)
target_link_libraries(cae_io PUBLIC cae_fem)

# heat equation.
add_library(cae_heat STATIC
    core/physics/heat/problem/Problem.cpp
    core/physics/heat/assembly/Assembler2D.cpp
    core/physics/heat/assembly/BoundaryAssembler2D.cpp
    core/physics/heat/HeatSolver.cpp
)
target_link_libraries(cae_heat PUBLIC cae_fem)



# Navier-Stokes.
add_library(cae_hydro STATIC
    core/physics/hydro/problem/HydroProblem.cpp
    core/physics/hydro/assembly/HydroAssembler.cpp
    core/physics/hydro/assembly/HydroBoundaryAssembler.cpp
    core/physics/hydro/HydroSolver.cpp
)
target_link_libraries(cae_hydro PUBLIC cae_fem cae_io)


# Deforamtion.
add_library(cae_deform STATIC
    core/physics/deformation/problem/Problem.cpp   
    core/physics/deformation/Solver.cpp    
    core/physics/deformation/assembly/Assembler.cpp
    core/physics/deformation/assembly/BoundaryAssembler.cpp
)

target_link_libraries(cae_deform PUBLIC cae_fem cae_io)

# Executable.
add_executable(cae_heat_app examples/mainHeat.cpp)
target_link_libraries(cae_heat_app PRIVATE cae_heat cae_io)


add_executable(cae_hydro_app examples/mainHydro.cpp)
target_link_libraries(cae_hydro_app PRIVATE cae_hydro cae_heat)


add_executable(cae_deform_app examples/mainDeformation.cpp)
target_link_libraries(cae_deform_app PRIVATE cae_deform)


set_target_properties(cae_heat_app PROPERTIES OUTPUT_NAME cae_heat)
set_target_properties(cae_hydro_app PROPERTIES OUTPUT_NAME cae_hydro)
set_target_properties(cae_deform_app PROPERTIES OUTPUT_NAME cae_deform)