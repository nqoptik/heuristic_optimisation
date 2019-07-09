/**
 * @file common.hpp
 * @author Nguyen Quang <nguyenquang.emailbox@gmail.com>
 * @brief Some common functions for heuristic optimisation.
 * @since 0.0.1
 * 
 * @copyright Copyright (c) 2016, Nguyen Quang, all rights reserved.
 * 
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

void generate_vertices(const std::string& file_name,
                       const int& height,
                       const int& width,
                       const int& step_size,
                       const int& border,
                       const int& number_of_vertices,
                       std::vector<std::vector<int>>& vertices);
void compute_cost_matrix(const std::string& file_name,
                         const int& number_of_vertices,
                         const std::vector<std::vector<int>>& vertices,
                         std::vector<std::vector<float>>& cost_matrix);

#endif // COMMON_HPP
