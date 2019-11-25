/**
 * @file genetic_algorithm.hpp
 * @author Nguyen Quang <nqoptik@gmail.com>
 * @brief The genetic algorithm.
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

void genetic(const int& number_of_vertices,
             const int& population_size,
             const int& hybridization_size,
             const int& mutation_size,
             const int& number_of_generations,
             const std::vector<std::vector<float>>& cost_matrix,
             std::vector<int>& tour,
             float& cost);
void initialise(const int& number_of_vertices,
                const int& population_size,
                const int& hybridization_size,
                const int& mutation_size,
                std::vector<std::vector<int>>& population);
void hybridise(const int& number_of_vertices,
               const std::vector<int>& tour1,
               const std::vector<int>& tour2,
               std::vector<int>& tour1_,
               std::vector<int>& tour2_,
               const int& from,
               const int& to);
void mutate(const int& number_of_vertices,
            const std::vector<int>& tour,
            std::vector<int>& tour_,
            const int& method,
            int& from,
            int& to);
void select(const int& number_of_vertices,
            const int& population_size,
            const int& hybridization_size,
            const int& mutation_size,
            const std::vector<std::vector<float>>& cost_matrix,
            std::vector<std::vector<int>>& population,
            std::vector<float>& current_costs);

int main()
{
    int height = 800;
    int width = 1200;
    int step_size = 8;
    int border = 25;
    int number_of_vertices = 150;
    std::vector<std::vector<int>> vertices;
    generate_vertices("vertices.data", height, width, step_size, border,
                      number_of_vertices, vertices);
    std::vector<std::vector<float>> cost_matrix;
    compute_cost_matrix("cost_matrix.data", number_of_vertices, vertices, cost_matrix);

    std::vector<int> genetic_tour;
    float genetic_algorithm_cost;
    int population_size = 500;
    int hybridization_size = 150;
    int mutation_size = 20;
    int number_of_generations = 20000;
    genetic(number_of_vertices, population_size, hybridization_size, mutation_size,
            number_of_generations, cost_matrix, genetic_tour, genetic_algorithm_cost);

    cv::Mat genetic_map(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < number_of_vertices; ++i)
    {
        cv::circle(genetic_map, cv::Point(vertices[i][0], vertices[i][1]),
                   1, cv::Scalar(0, 0, 255), 2, 0);
    }
    for (int i = 0; i < number_of_vertices - 1; ++i)
    {
        cv::line(genetic_map,
                 cv::Point(vertices[genetic_tour[i]][0], vertices[genetic_tour[i]][1]),
                 cv::Point(vertices[genetic_tour[i + 1]][0], vertices[genetic_tour[i + 1]][1]),
                 cv::Scalar(255, 0, 0), 1, 8, 0);
    }
    std::string genetic_text = "GA: ";
    genetic_text.append(std::to_string(genetic_algorithm_cost));
    cv::putText(genetic_map, genetic_text, cv::Point(border, height - border),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
    cv::imshow("genetic_map", genetic_map);
    cv::waitKey();

    return 0;
}

void genetic(const int& number_of_vertices,
             const int& population_size,
             const int& hybridization_size,
             const int& mutation_size,
             const int& number_of_generations,
             const std::vector<std::vector<float>>& cost_matrix,
             std::vector<int>& tour,
             float& cost)
{
    std::vector<std::vector<int>> population = std::vector<std::vector<int>>(population_size + hybridization_size * 2 + mutation_size);
    initialise(number_of_vertices, population_size, hybridization_size, mutation_size, population);
    for (int generation = 0; generation < number_of_generations; ++generation)
    {
        for (int i = 0; i < hybridization_size; ++i)
        {
            int first_individual_index = rand() % population_size;
            int second_individual_index = rand() % population_size;
            int from = rand() % number_of_vertices;
            int to = rand() % number_of_vertices;
            std::vector<int> tour1_, tour2_;
            hybridise(number_of_vertices, population[first_individual_index],
                      population[second_individual_index],
                      tour1_,
                      tour2_, from, to);
            for (int index = 0; index < number_of_vertices; ++index)
            {
                population[population_size + 2 * i][index] = tour1_[index];
                population[population_size + 2 * i + 1][index] = tour2_[index];
            }
        }

        for (int i = 0; i < mutation_size; ++i)
        {
            int individual_index = rand() % population_size;
            int method = rand() % 3;
            int from = rand() % number_of_vertices;
            int to = rand() % number_of_vertices;

            std::vector<int> tour_;
            mutate(number_of_vertices, population[individual_index],
                   tour_, method,
                   from, to);
            for (int index = 0; index < number_of_vertices; ++index)
            {
                population[population_size + 2 * hybridization_size + i][index] = tour_[index];
            }
        }

        std::vector<float> current_costs;
        select(number_of_vertices, population_size, hybridization_size, mutation_size,
               cost_matrix, population, current_costs);
        cost = current_costs[0];
    }
    for (int i = 0; i < number_of_vertices; ++i)
    {
        tour.push_back(population[0][i]);
    }
}

void initialise(const int& number_of_vertices,
                const int& population_size,
                const int& hybridization_size,
                const int& mutation_size,
                std::vector<std::vector<int>>& population)
{
    for (int i = 0; i < population_size + hybridization_size * 2 + mutation_size; ++i)
    {
        population[i] = std::vector<int>(number_of_vertices);
        std::vector<int> flag = std::vector<int>(number_of_vertices, 0);
        for (int j = 0; j < number_of_vertices; ++j)
        {
            int shifting_vertex = rand() % (number_of_vertices - j);

            int current_vertex = 0;
            while (flag[current_vertex] != 0)
            {
                ++current_vertex;
            }

            while (shifting_vertex != 0)
            {
                ++current_vertex;
                if (flag[current_vertex] == 0)
                {
                    shifting_vertex--;
                }
            }

            population[i][j] = current_vertex;
            flag[current_vertex] = 1;
        }
    }
}

void hybridise(const int& number_of_vertices,
               const std::vector<int>& tour1,
               const std::vector<int>& tour2,
               std::vector<int>& tour1_,
               std::vector<int>& tour2_,
               const int& from,
               const int& to)
{
    tour1_ = std::vector<int>(number_of_vertices);
    tour2_ = std::vector<int>(number_of_vertices);
    std::vector<int> flag = std::vector<int>(number_of_vertices, 0);
    std::vector<int> checkTour1 = std::vector<int>(number_of_vertices, 0);
    std::vector<int> checkTour2 = std::vector<int>(number_of_vertices, 0);

    if (from < to)
    {
        for (int i = from; i <= to; ++i)
        {
            flag[i] = 1;
        }
    }
    else
    {
        for (int i = to; i <= from; ++i)
        {
            flag[i] = 1;
        }
    }

    for (int i = 0; i < number_of_vertices; ++i)
    {
        if (flag[i] == 1)
        {
            tour1_[i] = tour2[i];
            tour2_[i] = tour1[i];
            for (int j = 0; j < number_of_vertices; ++j)
            {
                if (tour1[j] == tour2[i])
                {
                    checkTour1[j] = 1;
                }
                if (tour2[j] == tour1[i])
                {
                    checkTour2[j] = 1;
                }
            }
        }
    }

    int current_index_1 = 0;
    int current_index_2 = 0;
    for (int i = 0; i < number_of_vertices; ++i)
    {
        while (checkTour1[current_index_1] == 1)
        {
            ++current_index_1;
        }
        while (checkTour2[current_index_2] == 1)
        {
            ++current_index_2;
        }
        if (flag[i] == 0)
        {
            tour1_[i] = tour1[current_index_1];
            tour2_[i] = tour2[current_index_2];
            ++current_index_1;
            ++current_index_2;
        }
    }
}

void mutate(const int& number_of_vertices,
            const std::vector<int>& tour,
            std::vector<int>& tour_,
            const int& method,
            int& from,
            int& to)
{
    tour_ = std::vector<int>(number_of_vertices);

    if (from > to)
    {
        int temp = to;
        to = from;
        from = temp;
    }

    if (method == 0)
    {
        for (int i = 0; i < number_of_vertices; ++i)
        {
            tour_[i] = tour[i];
        }
        tour_[from] = tour[to];
        tour_[to] = tour[from];
    }
    else if (method == 1)
    {
        for (int i = 0; i < from; ++i)
        {
            tour_[i] = tour[i];
        }
        for (int i = from; i <= to; ++i)
        {
            tour_[i] = tour[from + to - i];
        }
        for (int i = to + 1; i < number_of_vertices; ++i)
        {
            tour_[i] = tour[i];
        }
    }
    else
    {
        for (int i = 0; i < from; ++i)
        {
            tour_[i] = tour[i];
        }
        for (int i = from; i < from + (number_of_vertices - (to + 1)); ++i)
        {
            tour_[i] = tour[i + to - from + 1];
        }
        for (int i = from + (number_of_vertices - (to + 1)); i < number_of_vertices; ++i)
        {
            tour_[i] = tour[i - (number_of_vertices - (to + 1))];
        }
    }
}

void select(const int& number_of_vertices,
            const int& population_size,
            const int& hybridization_size,
            const int& mutation_size,
            const std::vector<std::vector<float>>& cost_matrix,
            std::vector<std::vector<int>>& population,
            std::vector<float>& current_costs)
{
    int current_population_size = population_size + 2 * hybridization_size + mutation_size;
    current_costs = std::vector<float>(current_population_size);
    for (int i = 0; i < current_population_size; ++i)
    {
        float current_cost = 0;
        for (int j = 0; j < number_of_vertices; ++j)
        {
            current_cost += cost_matrix[population[i][j]][population[i][(j + 1) % number_of_vertices]];
        }
        current_costs[i] = current_cost;
    }

    for (int i = 0; i < current_population_size; ++i)
    {
        for (int j = i + 1; j < current_population_size; ++j)
        {
            if (current_costs[i] > current_costs[j])
            {
                float temporary_cost = current_costs[i];
                current_costs[i] = current_costs[j];
                current_costs[j] = temporary_cost;
                for (int index = 0; index < number_of_vertices; ++index)
                {
                    int tmp = population[i][index];
                    population[i][index] = population[j][index];
                    population[j][index] = tmp;
                }
            }
        }
    }
}
