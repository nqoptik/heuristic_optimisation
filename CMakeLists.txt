cmake_minimum_required(VERSION 3.1)

project(heuristic_optimisation)

## Compile as C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Compile with the highest warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

## System dependencies
find_package(OpenCV REQUIRED)

## Declare a C++ executable
add_executable(greedy_algorithm src/greedy_algorithm.cpp)

add_executable(ant_algorithm src/ant_algorithm.cpp)

add_executable(genetic_algorithm src/genetic_algorithm.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(greedy_algorithm ${OpenCV_LIBS})

target_link_libraries(ant_algorithm ${OpenCV_LIBS})

target_link_libraries(genetic_algorithm ${OpenCV_LIBS})
