cmake_minimum_required(VERSION 3.0.0)
project(sprint7_10_1 VERSION 0.1.0 LANGUAGES C CXX)
add_executable(sprint7_10_1 main.cpp array_ptr.h simple_vector.h tests.h)
target_compile_options(sprint7_10_1 PRIVATE -Wall -Wextra -Wpedantic -Werror)