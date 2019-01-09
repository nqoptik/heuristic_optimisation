cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(heuristic_optimization)

find_package(OpenCV REQUIRED)

add_executable(greedy_algorithm src/greedy_algorithm.cpp)
target_link_libraries(greedy_algorithm ${OpenCV_LIBS})

add_executable(ant_algorithm src/ant_algorithm.cpp)
target_link_libraries(ant_algorithm ${OpenCV_LIBS})

add_executable(genetic_algorithm src/genetic_algorithm.cpp)
target_link_libraries(genetic_algorithm ${OpenCV_LIBS})
