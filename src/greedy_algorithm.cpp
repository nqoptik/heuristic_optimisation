/**
 * @file greedy_algorithm.hpp
 * @author Nguyen Quang <nqoptik@gmail.com>
 * @brief The greedy algorithm.
 * @since 0.0.1
 * 
 * @copyright Copyright (c) 2016, Nguyen Quang, all rights reserved.
 * 
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "heuristic_optimisaion/common.hpp"

void greedy(const int& number_of_vertices,
            const int& start,
            const std::vector<std::vector<float>>& cost_matrix,
            std::vector<int>& tour,
            float& cost);

int main()
{
    int height = 800;
    int width = 1200;
    int step_size = 8;
    int border = 25;
    int number_of_vertices = 100;
    std::vector<std::vector<int>> vertices;
    generate_vertices("vertices.data", height, width, step_size, border,
                      number_of_vertices, vertices);
    std::vector<std::vector<float>> cost_matrix;
    compute_cost_matrix("cost_matrix.data", number_of_vertices, vertices, cost_matrix);

    int start = 0;
    std::vector<int> greedy_tour;
    float greedy_cost;
    greedy(number_of_vertices, start, cost_matrix, greedy_tour, greedy_cost);
    cv::Mat greedy_map(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < number_of_vertices; ++i)
    {
        cv::circle(greedy_map, cv::Point(vertices[i][0], vertices[i][1]),
                   1, cv::Scalar(0, 0, 255), 2, 0);
        cv::imshow("Cities", greedy_map);
        cv::waitKey(1);
    }
    for (int i = 0; i < number_of_vertices - 1; ++i)
    {
        cv::line(greedy_map,
                 cv::Point(vertices[greedy_tour[i]][0], vertices[greedy_tour[i]][1]),
                 cv::Point(vertices[greedy_tour[i + 1]][0], vertices[greedy_tour[i + 1]][1]),
                 cv::Scalar(255, 0, 0), 1, 8, 0);
        cv::imshow("Greedy", greedy_map);
        cv::waitKey(1);
    }
    std::string greedy_text = "number_of_vertices = ";
    greedy_text.append(std::to_string(number_of_vertices));
    greedy_text.append("     greedy: ");
    greedy_text.append(std::to_string(greedy_cost));
    cv::putText(greedy_map, greedy_text, cv::Point(border, height - border),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
    cv::imshow("Greedy", greedy_map);
    cv::imwrite("greedy.png", greedy_map);
    cv::waitKey();

    return 0;
}

void greedy(const int& number_of_vertices,
            const int& start,
            const std::vector<std::vector<float>>& cost_matrix,
            std::vector<int>& tour,
            float& cost)
{
    std::vector<bool> has_been_visited = std::vector<bool>(number_of_vertices, false);
    tour = std::vector<int>(number_of_vertices);
    tour[0] = start;
    has_been_visited[start] = true;
    int current_vertex = start;
    cost = 0;
    for (int step = 1; step < number_of_vertices; ++step)
    {
        float min_cost = FLT_MAX;
        int nearest_vertex = 0;
        for (int next_vertex = 0; next_vertex < number_of_vertices; ++next_vertex)
        {
            if (min_cost > cost_matrix[current_vertex][next_vertex] &&
                has_been_visited[next_vertex] == false)
            {
                min_cost = cost_matrix[current_vertex][next_vertex];
                nearest_vertex = next_vertex;
            }
        }
        tour[step] = nearest_vertex;
        has_been_visited[nearest_vertex] = true;
        current_vertex = nearest_vertex;
        cost += min_cost;
    }
    cost += cost_matrix[tour[number_of_vertices - 1]][start];
}
