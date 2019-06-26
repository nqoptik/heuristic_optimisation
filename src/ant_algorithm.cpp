#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void generate_vertices(std::string file_name,
                       int height,
                       int width,
                       int step_size,
                       int border,
                       int number_of_vertices,
                       std::vector<std::vector<int>>& vertices);
void compute_cost_matrix(std::string file_name,
                         int number_of_vertices,
                         std::vector<std::vector<int>> vertices,
                         std::vector<std::vector<float>>& cost_matrix);
void ant(int number_of_vertices,
         int number_of_ants,
         float evaporation,
         std::vector<std::vector<int>> vertices,
         int width,
         int height,
         int border,
         std::vector<std::vector<float>> cost_matrix,
         std::vector<int>& tour,
         float& cost);

int main() {
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

    int number_of_ants = 50000000;
    float evaporation = 0.8;
    std::vector<int> ant_tour;
    float ant_cost;

    ant(number_of_vertices, number_of_ants, evaporation, vertices, width, height, border,
        cost_matrix, ant_tour, ant_cost);
    cv::waitKey();

    return 0;
}

void generate_vertices(std::string file_name,
                       int height,
                       int width,
                       int step_size,
                       int border,
                       int number_of_vertices,
                       std::vector<std::vector<int>>& vertices) {
    vertices = std::vector<std::vector<int>>(number_of_vertices);
    std::ofstream fs;
    fs.open(file_name.c_str());
    fs << number_of_vertices << std::endl;
    for (int i = 0; i < number_of_vertices; ++i) {
        vertices[i] = std::vector<int>(2);
        int x = rand() % ((width - border * 3) / step_size) * step_size + border;
        int y = rand() % ((height - border * 3) / step_size) * step_size + border;
        vertices[i][0] = x;
        vertices[i][1] = y;
        fs << vertices[i][0] << " " << vertices[i][1] << std::endl;
    }
    fs.close();
}

void compute_cost_matrix(std::string file_name,
                         int number_of_vertices,
                         std::vector<std::vector<int>> vertices,
                         std::vector<std::vector<float>>& cost_matrix) {
    cost_matrix = std::vector<std::vector<float>>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i) {
        cost_matrix[i] = std::vector<float>(number_of_vertices);
        for (int j = 0; j < number_of_vertices; ++j) {
            if (i == j) {
                cost_matrix[i][j] = 99999;
            } else {
                int dx = vertices[i][0] - vertices[j][0];
                int dy = vertices[i][1] - vertices[j][1];
                cost_matrix[i][j] = sqrt(dx * dx + dy * dy);
            }
        }
    }

    std::ofstream fs;
    fs.open(file_name.c_str());
    fs << number_of_vertices << std::endl;
    for (int i = 0; i < number_of_vertices; ++i) {
        for (int j = 0; j < number_of_vertices; ++j) {
            fs << cost_matrix[i][j] << " ";
        }
        fs << std::endl;
    }
    fs.close();
}

void initialise_ant_algorithm(int number_of_vertices,
                              float initial_pheromone,
                              float scale_factor,
                              std::vector<std::vector<float>> cost_matrix,
                              std::vector<std::vector<float>>& pheromone,
                              std::vector<std::vector<float>>& heuristic_factors);

void update_pheromone(int number_of_vertices,
                      std::vector<int> path,
                      float scale_factor,
                      std::vector<std::vector<float>> cost_matrix,
                      float evaporation,
                      std::vector<std::vector<float>>& pheromone);

void ant(int number_of_vertices,
         int number_of_ants,
         float evaporation,
         std::vector<std::vector<int>> vertices,
         int width,
         int height,
         int border,
         std::vector<std::vector<float>> cost_matrix,
         std::vector<int>& tour,
         float& cost) {
    cost = 99999;

    tour = std::vector<int>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i) {
        tour[i] = 0;
    }
    float initial_pheromone = 1.0f / static_cast<float>(number_of_vertices);
    float scale_factor = 1;
    for (int i = 0; i < number_of_vertices; ++i) {
        for (int j = 0; j < number_of_vertices; ++j) {
            if (scale_factor < cost_matrix[i][j] && i != j) {
                scale_factor = cost_matrix[i][j];
            }
        }
    }
    std::cout << "scale factor  " << scale_factor << std::endl;
    std::vector<std::vector<float>> pheromone;
    std::vector<std::vector<float>> heuristic_factors;
    initialise_ant_algorithm(number_of_vertices, initial_pheromone, scale_factor, cost_matrix, pheromone,
                             heuristic_factors);
    std::vector<int> flag = std::vector<int>(number_of_vertices);
    std::vector<float> probability = std::vector<float>(number_of_vertices);
    int stop_count = 0;
    for (int ant = 0; ant < number_of_ants; ++ant) {
        for (int i = 0; i < number_of_vertices; ++i) {
            flag[i] = 0;
        }
        int start = rand() % number_of_vertices;
        std::vector<int> path = std::vector<int>(number_of_vertices);
        path[0] = start;
        flag[start] = 1;
        int current_vertex = start;

        for (int step = 1; step < number_of_vertices; ++step) {
            int selected_vertex = 0;

            for (int i = 0; i < number_of_vertices; ++i) {
                probability[i] = 0;
            }

            float sumP = 0;
            for (int next_vertex = 0; next_vertex < number_of_vertices; ++next_vertex) {
                if (flag[next_vertex] == 0) {
                    probability[next_vertex] = pheromone[current_vertex][next_vertex] *
                                               heuristic_factors[current_vertex][next_vertex];
                    sumP += probability[next_vertex];
                }
            }
            for (int next_vertex = 0; next_vertex < number_of_vertices; ++next_vertex) {
                probability[next_vertex] /= sumP;
            }

            for (int next_vertex = 1; next_vertex < number_of_vertices; ++next_vertex) {
                probability[next_vertex] += probability[next_vertex - 1];
            }

            float roulette = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (roulette < probability[0]) {
                selected_vertex = 0;
            } else {
                for (int next_vertex = 1; next_vertex < number_of_vertices; ++next_vertex) {
                    if (roulette >= probability[next_vertex - 1] &&
                        roulette < probability[next_vertex]) {
                        selected_vertex = next_vertex;
                    }
                }
            }

            path[step] = selected_vertex;
            flag[selected_vertex] = 1;
            current_vertex = selected_vertex;
        }

        if (ant % 100 == 99) {
            path = tour;
        }

        update_pheromone(number_of_vertices, path, scale_factor, cost_matrix,
                         evaporation, pheromone);

        float current_cost = 0;
        for (int i = 1; i < number_of_vertices; ++i) {
            current_cost += cost_matrix[path[i - 1]][path[i]];
        }
        current_cost += cost_matrix[path[number_of_vertices - 1]][path[0]];
        std::cout << current_cost << std::endl;

        if (current_cost < cost) {
            cost = current_cost;
            tour = path;

            cv::Mat ant_map(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

            for (int i = 0; i < number_of_vertices; ++i) {
                cv::circle(ant_map, cv::Point(vertices[i][0], vertices[i][1]),
                           1, cv::Scalar(0, 0, 255), 2, 0);
            }
            for (int i = 0; i < number_of_vertices - 1; ++i) {
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
        } else {
            ++stop_count;
            cv::waitKey(1);
        }
        if (stop_count > 10000 * number_of_vertices) {
            break;
        }
    }
}

void initialise_ant_algorithm(int number_of_vertices,
                              float initial_pheromone,
                              float scale_factor,
                              std::vector<std::vector<float>> cost_matrix,
                              std::vector<std::vector<float>>& pheromone,
                              std::vector<std::vector<float>>& heuristic_factors) {
    pheromone = std::vector<std::vector<float>>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i) {
        pheromone[i] = std::vector<float>(number_of_vertices);
        for (int j = 0; j < number_of_vertices; ++j) {
            pheromone[i][j] = initial_pheromone;
        }
    }

    heuristic_factors = std::vector<std::vector<float>>(number_of_vertices);
    for (int i = 0; i < number_of_vertices; ++i) {
        heuristic_factors[i] = std::vector<float>(number_of_vertices);
        for (int j = 0; j < number_of_vertices; ++j) {
            if (cost_matrix[i][j] == 0) {
                heuristic_factors[i][j] = scale_factor;
            } else {
                heuristic_factors[i][j] = scale_factor / cost_matrix[i][j];
            }
        }
    }
}

void update_pheromone(int number_of_vertices,
                      std::vector<int> path,
                      float scale_factor,
                      std::vector<std::vector<float>> cost_matrix,
                      float evaporation,
                      std::vector<std::vector<float>>& pheromone) {
    float current_cost = 0;
    for (int i = 1; i < number_of_vertices; ++i) {
        current_cost += cost_matrix[path[i - 1]][path[i]];
    }
    current_cost += cost_matrix[path[number_of_vertices - 1]][path[0]];

    float delta = scale_factor / current_cost;

    for (int i = 1; i < number_of_vertices; ++i) {
        int from = path[i - 1];
        int to = path[i];
        pheromone[from][to] += delta;
        pheromone[to][from] += delta;
    }

    pheromone[path[number_of_vertices - 1]][path[0]] += delta;
    pheromone[path[0]][path[number_of_vertices - 1]] += delta;

    for (int i = 0; i < number_of_vertices; ++i) {
        for (int j = 0; j < number_of_vertices; ++j) {
            pheromone[i][j] = evaporation * pheromone[i][j];
        }
    }
}
