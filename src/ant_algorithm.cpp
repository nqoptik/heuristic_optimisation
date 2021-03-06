/**
 * @file ant_algorithm.hpp
 * @author Nguyen Quang <nqoptik@gmail.com>
 * @brief The ant algorithm.
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

void ant(const int& number_of_vertices,
         const int& number_of_ants,
         const float& evaporation,
         const std::vector<std::vector<int>>& vertices,
         const int& width,
         const int& height,
         const int& border,
         const std::vector<std::vector<float>>& cost_matrix,
         std::vector<int>& tour,
         float& cost);
void initialise_ant_algorithm(const int& number_of_vertices,
                              const float& initial_pheromone,
                              const float& scale_factor,
                              const std::vector<std::vector<float>>& cost_matrix,
                              std::vector<std::vector<float>>& pheromone,
                              std::vector<std::vector<float>>& heuristic_factors);

void update_pheromone(const int& number_of_vertices,
                      const std::vector<int>& path,
                      const float& scale_factor,
                      const std::vector<std::vector<float>>& cost_matrix,
                      const float& evaporation,
                      std::vector<std::vector<float>>& pheromone);

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

    int number_of_ants = 50000000;
    float evaporation = 0.8;
    std::vector<int> ant_tour;
    float ant_cost;

    ant(number_of_vertices, number_of_ants, evaporation, vertices, width, height, border, cost_matrix, ant_tour, ant_cost);
    cv::waitKey();

    return 0;
}

void ant(const int& number_of_vertices,
         const int& number_of_ants,
         const float& evaporation,
         const std::vector<std::vector<int>>& vertices,
         const int& width,
         const int& height,
         const int& border,
         const std::vector<std::vector<float>>& cost_matrix,
         std::vector<int>& tour,
         float& cost)
{
    cost = FLT_MAX;

    tour = std::vector<int>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i)
    {
        tour[i] = 0;
    }
    float initial_pheromone = 1.0f / static_cast<float>(number_of_vertices);
    float scale_factor = 1;
    for (int i = 0; i < number_of_vertices; ++i)
    {
        for (int j = 0; j < number_of_vertices; ++j)
        {
            if (scale_factor < cost_matrix[i][j] && i != j)
            {
                scale_factor = cost_matrix[i][j];
            }
        }
    }
    std::cout << "scale factor  " << scale_factor << std::endl;
    std::vector<std::vector<float>> pheromone;
    std::vector<std::vector<float>> heuristic_factors;
    initialise_ant_algorithm(number_of_vertices, initial_pheromone, scale_factor, cost_matrix, pheromone,
                             heuristic_factors);
    std::vector<bool> has_been_visited = std::vector<bool>(number_of_vertices);
    std::vector<float> probability = std::vector<float>(number_of_vertices);
    int stop_count = 0;
    for (int ant = 0; ant < number_of_ants; ++ant)
    {
        for (int i = 0; i < number_of_vertices; ++i)
        {
            has_been_visited[i] = false;
        }
        int start = rand() % number_of_vertices;
        std::vector<int> path = std::vector<int>(number_of_vertices);
        path[0] = start;
        has_been_visited[start] = true;
        int current_vertex = start;

        for (int step = 1; step < number_of_vertices; ++step)
        {
            int selected_vertex = 0;

            for (int i = 0; i < number_of_vertices; ++i)
            {
                probability[i] = 0;
            }

            float sum_probability = 0;
            for (int next_vertex = 0; next_vertex < number_of_vertices; ++next_vertex)
            {
                if (has_been_visited[next_vertex] == false)
                {
                    probability[next_vertex] = pheromone[current_vertex][next_vertex] *
                                               heuristic_factors[current_vertex][next_vertex];
                    sum_probability += probability[next_vertex];
                }
            }
            for (int next_vertex = 0; next_vertex < number_of_vertices; ++next_vertex)
            {
                probability[next_vertex] /= sum_probability;
            }

            for (int next_vertex = 1; next_vertex < number_of_vertices; ++next_vertex)
            {
                probability[next_vertex] += probability[next_vertex - 1];
            }

            float roulette = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (roulette < probability[0])
            {
                selected_vertex = 0;
            }
            else
            {
                for (int next_vertex = 1; next_vertex < number_of_vertices; ++next_vertex)
                {
                    if (roulette >= probability[next_vertex - 1] &&
                        roulette < probability[next_vertex])
                    {
                        selected_vertex = next_vertex;
                    }
                }
            }

            path[step] = selected_vertex;
            has_been_visited[selected_vertex] = true;
            current_vertex = selected_vertex;
        }

        if (ant % 100 == 99)
        {
            path = tour;
        }

        update_pheromone(number_of_vertices, path, scale_factor, cost_matrix,
                         evaporation, pheromone);

        float current_cost = 0;
        for (int i = 1; i < number_of_vertices; ++i)
        {
            current_cost += cost_matrix[path[i - 1]][path[i]];
        }
        current_cost += cost_matrix[path[number_of_vertices - 1]][path[0]];
        std::cout << current_cost << std::endl;

        if (current_cost < cost)
        {
            cost = current_cost;
            tour = path;

            cv::Mat ant_map(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

            for (int i = 0; i < number_of_vertices; ++i)
            {
                cv::circle(ant_map, cv::Point(vertices[i][0], vertices[i][1]),
                           1, cv::Scalar(0, 0, 255), 2, 0);
            }
            for (int i = 0; i < number_of_vertices - 1; ++i)
            {
                cv::line(ant_map,
                         cv::Point(vertices[tour[i]][0], vertices[tour[i]][1]),
                         cv::Point(vertices[tour[i + 1]][0], vertices[tour[i + 1]][1]),
                         cv::Scalar(255, 0, 0), 1, 8, 0);
            }
            std::string ant_text = "number_of_vertices: ";
            ant_text.append(std::to_string(number_of_vertices));
            ant_text.append(" ant: ");
            ant_text.append(std::to_string(cost));
            ant_text.append(" step: ");
            ant_text.append(std::to_string(ant));
            cv::putText(ant_map, ant_text, cv::Point(border, height - border),
                        cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
            cv::imshow("ant Tour", ant_map);
            cv::waitKey(250);
            cv::imwrite("ant.png", ant_map);
            stop_count = 0;
        }
        else
        {
            ++stop_count;
            cv::waitKey(1);
        }
        if (stop_count > 10000 * number_of_vertices)
        {
            break;
        }
    }
}

void initialise_ant_algorithm(const int& number_of_vertices,
                              const float& initial_pheromone,
                              const float& scale_factor,
                              const std::vector<std::vector<float>>& cost_matrix,
                              std::vector<std::vector<float>>& pheromone,
                              std::vector<std::vector<float>>& heuristic_factors)
{
    pheromone = std::vector<std::vector<float>>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i)
    {
        pheromone[i] = std::vector<float>(number_of_vertices);
        for (int j = 0; j < number_of_vertices; ++j)
        {
            pheromone[i][j] = initial_pheromone;
        }
    }

    heuristic_factors = std::vector<std::vector<float>>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i)
    {
        heuristic_factors[i] = std::vector<float>(number_of_vertices);
        for (int j = 0; j < number_of_vertices; ++j)
        {
            if (cost_matrix[i][j] == 0)
            {
                heuristic_factors[i][j] = scale_factor;
            }
            else
            {
                heuristic_factors[i][j] = scale_factor / cost_matrix[i][j];
            }
        }
    }
}

void update_pheromone(const int& number_of_vertices,
                      const std::vector<int>& path,
                      const float& scale_factor,
                      const std::vector<std::vector<float>>& cost_matrix,
                      const float& evaporation,
                      std::vector<std::vector<float>>& pheromone)
{
    float current_cost = 0;
    for (int i = 1; i < number_of_vertices; ++i)
    {
        current_cost += cost_matrix[path[i - 1]][path[i]];
    }
    current_cost += cost_matrix[path[number_of_vertices - 1]][path[0]];

    float delta = scale_factor / current_cost;

    for (int i = 1; i < number_of_vertices; ++i)
    {
        int from = path[i - 1];
        int to = path[i];
        pheromone[from][to] += delta;
        pheromone[to][from] += delta;
    }

    pheromone[path[number_of_vertices - 1]][path[0]] += delta;
    pheromone[path[0]][path[number_of_vertices - 1]] += delta;

    for (int i = 0; i < number_of_vertices; ++i)
    {
        for (int j = 0; j < number_of_vertices; ++j)
        {
            pheromone[i][j] = evaporation * pheromone[i][j];
        }
    }
}
