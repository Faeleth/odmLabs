cmake_minimum_required(VERSION 3.10)
project(odmlab1 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(odmlab1 main.cpp)

set_target_properties(odmlab1 PROPERTIES OUTPUT_NAME "odmlab1")
set_target_properties(odmlab1 PROPERTIES SUFFIX ".out")

