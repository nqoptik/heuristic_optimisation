#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void generateVertices(std::string fileName, int height, int width, int stepSize, int border, int numOfVertices, std::vector<std::vector<int>>& vertices);
void computeCostMatrix(std::string fileName, int numOfVertices, std::vector<std::vector<int>> vertices, std::vector<std::vector<float>>& costMatrix);
void ant(int numOfVertices, int numOfAnts, float evaporation, std::vector<std::vector<int>> vertices, int width, int height, int border, std::vector<std::vector<float>> costMatrix, std::vector<int>& tour, float& cost);

int main(int argc, char** argv) {
    int height = 800;
    int width = 1200;
    int stepSize = 8;
    int border = 25;
    int numOfVertices = 150;
    std::vector<std::vector<int>> vertices;
    generateVertices("vertices.data", height, width, stepSize, border,
                     numOfVertices, vertices);
    std::vector<std::vector<float>> costMatrix;
    computeCostMatrix("costMatrix.data", numOfVertices, vertices, costMatrix);

    int numOfAnts = 50000000;
    float evaporation = 0.8;
    std::vector<int> antTour;
    float antCost;

    ant(numOfVertices, numOfAnts, evaporation, vertices, width, height, border,
        costMatrix, antTour, antCost);
    cv::waitKey();

    return 0;
}

void generateVertices(std::string fileName, int height, int width, int stepSize, int border, int numOfVertices, std::vector<std::vector<int>>& vertices) {
    vertices = std::vector<std::vector<int>>(numOfVertices);
    std::ofstream fs;
    fs.open(fileName.c_str());
    fs << numOfVertices << std::endl;
    for (int i = 0; i < numOfVertices; i++) {
        vertices[i] = std::vector<int>(2);
        int x = rand() % ((width - border * 3) / stepSize) * stepSize + border;
        int y = rand() % ((height - border * 3) / stepSize) * stepSize + border;
        vertices[i][0] = x;
        vertices[i][1] = y;
        fs << vertices[i][0] << " " << vertices[i][1] << std::endl;
    }
    fs.close();
}

void computeCostMatrix(std::string fileName, int numOfVertices, std::vector<std::vector<int>> vertices, std::vector<std::vector<float>>& costMatrix) {
    costMatrix = std::vector<std::vector<float>>(numOfVertices);
    for (int i = 0; i < numOfVertices; i++) {
        costMatrix[i] = std::vector<float>(numOfVertices);
        for (int j = 0; j < numOfVertices; j++) {
            if (i == j) {
                costMatrix[i][j] = 99999;
            } else {
                int dx = vertices[i][0] - vertices[j][0];
                int dy = vertices[i][1] - vertices[j][1];
                costMatrix[i][j] = sqrt(dx * dx + dy * dy);
            }
        }
    }

    std::ofstream fs;
    fs.open(fileName.c_str());
    fs << numOfVertices << std::endl;
    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            fs << costMatrix[i][j] << " ";
        }
        fs << std::endl;
    }
    fs.close();
}

void initAnt(int numOfVertices, float initialPheromone, float scaleFactor, std::vector<std::vector<float>> costMatrix, std::vector<std::vector<float>>& pheromone, std::vector<std::vector<float>>& heuristicFactors);
void updatePheromone(int numOfVertices, std::vector<int> path, float scaleFactor, std::vector<std::vector<float>> costMatrix, float evaporation, std::vector<std::vector<float>>& pheromone);

void ant(int numOfVertices, int numOfAnts, float evaporation, std::vector<std::vector<int>> vertices, int width, int height, int border, std::vector<std::vector<float>> costMatrix, std::vector<int>& tour, float& cost) {
    cost = 99999;

    tour = std::vector<int>(numOfVertices);
    for (int i = 0; i < numOfVertices; i++) {
        tour[i] = 0;
    }
    float initialPheromone = 1.0f / static_cast<float>(numOfVertices);
    float scaleFactor = 1;
    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            if (scaleFactor < costMatrix[i][j] && i != j) {
                scaleFactor = costMatrix[i][j];
            }
        }
    }
    std::cout << "scale factor  " << scaleFactor << std::endl;
    std::vector<std::vector<float>> pheromone;
    std::vector<std::vector<float>> heuristicFactors;
    initAnt(numOfVertices, initialPheromone, scaleFactor, costMatrix, pheromone,
            heuristicFactors);
    std::vector<int> flag = std::vector<int>(numOfVertices);
    std::vector<float> probability = std::vector<float>(numOfVertices);
    int stop_count = 0;
    for (int ant = 0; ant < numOfAnts; ant++) {
        for (int i = 0; i < numOfVertices; i++) {
            flag[i] = 0;
        }
        int start = rand() % numOfVertices;
        std::vector<int> path = std::vector<int>(numOfVertices);
        path[0] = start;
        flag[start] = 1;
        int currentVertex = start;

        for (int step = 1; step < numOfVertices; step++) {
            int selectedVertex;

            for (int i = 0; i < numOfVertices; i++) {
                probability[i] = 0;
            }

            float sumP = 0;
            for (int nextVertex = 0; nextVertex < numOfVertices; nextVertex++) {
                if (flag[nextVertex] == 0) {
                    probability[nextVertex] = pheromone[currentVertex][nextVertex] *
                                              heuristicFactors[currentVertex][nextVertex];
                    sumP += probability[nextVertex];
                }
            }
            for (int nextVertex = 0; nextVertex < numOfVertices; nextVertex++) {
                probability[nextVertex] /= sumP;
            }

            for (int nextVertex = 1; nextVertex < numOfVertices; nextVertex++) {
                probability[nextVertex] += probability[nextVertex - 1];
            }

            float Roulette = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (Roulette < probability[0]) {
                selectedVertex = 0;
            } else {
                for (int nextVertex = 1; nextVertex < numOfVertices; nextVertex++) {
                    if (Roulette >= probability[nextVertex - 1] &&
                        Roulette < probability[nextVertex]) {
                        selectedVertex = nextVertex;
                    }
                }
            }

            path[step] = selectedVertex;
            flag[selectedVertex] = 1;
            currentVertex = selectedVertex;
        }

        if (ant % 100 == 99) {
            path = tour;
        }

        updatePheromone(numOfVertices, path, scaleFactor, costMatrix,
                        evaporation, pheromone);

        float currentCost = 0;
        for (int i = 1; i < numOfVertices; i++) {
            currentCost += costMatrix[path[i - 1]][path[i]];
        }
        currentCost += costMatrix[path[numOfVertices - 1]][path[0]];
        std::cout << currentCost << std::endl;

        if (currentCost < cost) {
            cost = currentCost;
            tour = path;

            cv::Mat antMap(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

            for (int i = 0; i < numOfVertices; i++) {
                cv::circle(antMap, cv::Point(vertices[i][0], vertices[i][1]),
                           1, cv::Scalar(0, 0, 255), 2, 0);
            }
            for (int i = 0; i < numOfVertices - 1; i++) {
                cv::line(antMap,
                         cv::Point(vertices[tour[i]][0], vertices[tour[i]][1]),
                         cv::Point(vertices[tour[i + 1]][0], vertices[tour[i + 1]][1]),
                         cv::Scalar(255, 0, 0), 1, 8, 0);
            }
            std::string antText = "numOfVertices: ";
            antText.append(std::to_string(numOfVertices));
            antText.append(" ant: ");
            antText.append(std::to_string(cost));
            antText.append(" step: ");
            antText.append(std::to_string(ant));
            cv::putText(antMap, antText, cv::Point(border, height - border),
                        cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
            cv::imshow("ant Tour", antMap);
            cv::waitKey(250);
            cv::imwrite("ant.png", antMap);
            stop_count = 0;
        } else {
            stop_count++;
            cv::waitKey(1);
        }
        if (stop_count > 10000 * numOfVertices) {
            break;
        }
    }
}

void initAnt(int numOfVertices, float initialPheromone, float scaleFactor, std::vector<std::vector<float>> costMatrix, std::vector<std::vector<float>>& pheromone, std::vector<std::vector<float>>& heuristicFactors) {
    pheromone = std::vector<std::vector<float>>(numOfVertices);
    for (int i = 0; i < numOfVertices; i++) {
        pheromone[i] = std::vector<float>(numOfVertices);
        for (int j = 0; j < numOfVertices; j++) {
            pheromone[i][j] = initialPheromone;
        }
    }

    heuristicFactors = std::vector<std::vector<float>>(numOfVertices);
    for (int i = 0; i < numOfVertices; i++) {
        heuristicFactors[i] = std::vector<float>(numOfVertices);
        for (int j = 0; j < numOfVertices; j++) {
            if (costMatrix[i][j] == 0) {
                heuristicFactors[i][j] = scaleFactor;
            } else {
                heuristicFactors[i][j] = scaleFactor / costMatrix[i][j];
            }
        }
    }
}

void updatePheromone(int numOfVertices, std::vector<int> path, float scaleFactor, std::vector<std::vector<float>> costMatrix, float evaporation, std::vector<std::vector<float>>& pheromone) {
    float currentCost = 0;
    for (int i = 1; i < numOfVertices; i++) {
        currentCost += costMatrix[path[i - 1]][path[i]];
    }
    currentCost += costMatrix[path[numOfVertices - 1]][path[0]];

    float delta = scaleFactor / currentCost;

    for (int i = 1; i < numOfVertices; i++) {
        int from = path[i - 1];
        int to = path[i];
        pheromone[from][to] += delta;
        pheromone[to][from] += delta;
    }

    pheromone[path[numOfVertices - 1]][path[0]] += delta;
    pheromone[path[0]][path[numOfVertices - 1]] += delta;

    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            pheromone[i][j] = evaporation * pheromone[i][j];
        }
    }
}
