cmake_minimum_required(VERSION 3.30)

project(MyVector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(mv.out main.cpp AdvancedVector.cpp)

