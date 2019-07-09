/**
 * @file common.hpp
 * @author Nguyen Quang <nguyenquang.emailbox@gmail.com>
 * @brief Some common functions for heuristic optimisation.
 * @since 0.0.1
 * 
 * @copyright Copyright (c) 2016, Nguyen Quang, all rights reserved.
 * 
 */

#include "heuristic_optimisaion/common.hpp"

void generate_vertices(const std::string& file_name,
                       const int& height,
                       const int& width,
                       const int& step_size,
                       const int& border,
                       const int& number_of_vertices,
                       std::vector<std::vector<int>>& vertices)
{
    vertices = std::vector<std::vector<int>>(number_of_vertices);
    std::ofstream fs;
    fs.open(file_name.c_str());
    fs << number_of_vertices << std::endl;
    for (int i = 0; i < number_of_vertices; ++i)
    {
        vertices[i] = std::vector<int>(2);
        int x = rand() % ((width - border * 3) / step_size) * step_size + border;
        int y = rand() % ((height - border * 3) / step_size) * step_size + border;
        vertices[i][0] = x;
        vertices[i][1] = y;
        fs << vertices[i][0] << " " << vertices[i][1] << std::endl;
    }
    fs.close();
}

void compute_cost_matrix(const std::string& file_name,
                         const int& number_of_vertices,
                         const std::vector<std::vector<int>>& vertices,
                         std::vector<std::vector<float>>& cost_matrix)
{
    cost_matrix = std::vector<std::vector<float>>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i)
    {
        cost_matrix[i] = std::vector<float>(number_of_vertices);
        for (int j = 0; j < number_of_vertices; ++j)
        {
            if (i == j)
            {
                cost_matrix[i][j] = FLT_MAX;
            }
            else
            {
                int dx = vertices[i][0] - vertices[j][0];
                int dy = vertices[i][1] - vertices[j][1];
                cost_matrix[i][j] = sqrt(dx * dx + dy * dy);
            }
        }
    }

    std::ofstream fs;
    fs.open(file_name.c_str());
    fs << number_of_vertices << std::endl;
    for (int i = 0; i < number_of_vertices; ++i)
    {
        for (int j = 0; j < number_of_vertices; ++j)
        {
            fs << cost_matrix[i][j] << " ";
        }
        fs << std::endl;
    }
    fs.close();
}
