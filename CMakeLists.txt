cmake_minimum_required(VERSION 3.0.0)

project(calculator)

add_executable(calculator calculator.cpp)
target_compile_features(calculator PRIVATE cxx_std_17)
