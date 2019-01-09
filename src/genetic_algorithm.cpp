#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void generateVertices(std::string fileName, int height, int width, int stepSize, int border, int numOfVertices, std::vector<std::vector<int>>& vertices);
void computeCostMatrix(std::string fileName, int numOfVertices, std::vector<std::vector<int>> vertices, std::vector<std::vector<float>>& costMatrix);
void genetic(int numOfVertices, int populationSize, int hybridizationSize, int mutationSize, int numOfGenerations, std::vector<std::vector<float>> costMatrix, std::vector<int>& tour, float& cost);

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

    std::vector<int> geneticTour;
    float GACost;
    int populationSize = 500;
    int hybridizationSize = 150;
    int mutationSize = 20;
    int numOfGenerations = 20000;
    genetic(numOfVertices, populationSize, hybridizationSize, mutationSize,
            numOfGenerations, costMatrix, geneticTour, GACost);

    cv::Mat geneticMap(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < numOfVertices; i++) {
        cv::circle(geneticMap, cv::Point(vertices[i][0], vertices[i][1]),
                   1, cv::Scalar(0, 0, 255), 2, 0);
    }
    for (int i = 0; i < numOfVertices - 1; i++) {
        cv::line(geneticMap,
                 cv::Point(vertices[geneticTour[i]][0], vertices[geneticTour[i]][1]),
                 cv::Point(vertices[geneticTour[i + 1]][0], vertices[geneticTour[i + 1]][1]),
                 cv::Scalar(255, 0, 0), 1, 8, 0);
    }
    std::string geneticText = "GA: ";
    geneticText.append(std::to_string(GACost));
    cv::putText(geneticMap, geneticText, cv::Point(border, height - border),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
    cv::imshow("geneticMap", geneticMap);
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

void initialization(int numOfVertices, int populationSize, int hybridizationSize, int mutationSize, std::vector<std::vector<int>>& population);
void hybridization(int numOfVertices, std::vector<int> tour1, std::vector<int> tour2, std::vector<int>& tour1_, std::vector<int>& tour2_, int from, int to);
void mutation(int numOfVertices, std::vector<int> tour, std::vector<int>& tour_, int method, int from, int to);
void selection(int numOfVertices, int populationSize, int hybridizationSize, int mutationSize, std::vector<std::vector<float>> costMatrix, std::vector<std::vector<int>>& population, std::vector<float>& currentCosts);

void genetic(int numOfVertices, int populationSize, int hybridizationSize, int mutationSize, int numOfGenerations, std::vector<std::vector<float>> costMatrix, std::vector<int>& tour, float& cost) {
    std::vector<std::vector<int>> population = std::vector<std::vector<int>>(populationSize + hybridizationSize * 2 + mutationSize);
    initialization(numOfVertices, populationSize, hybridizationSize, mutationSize, population);
    for (int generation = 0; generation < numOfGenerations; generation++) {
        for (int i = 0; i < hybridizationSize; i++) {
            int firstIndividualIndex = rand() % populationSize;
            int secondIndividualIndex = rand() % populationSize;
            int from = rand() % numOfVertices;
            int to = rand() % numOfVertices;
            std::vector<int> tour1_, tour2_;
            hybridization(numOfVertices, population[firstIndividualIndex],
                          population[secondIndividualIndex],
                          tour1_,
                          tour2_, from, to);
            for (int idx = 0; idx < numOfVertices; idx++) {
                population[populationSize + 2 * i][idx] = tour1_[idx];
                population[populationSize + 2 * i + 1][idx] = tour2_[idx];
            }
        }

        for (int i = 0; i < mutationSize; i++) {
            int individualIndex = rand() % populationSize;
            int method = rand() % 3;
            int from = rand() % numOfVertices;
            int to = rand() % numOfVertices;

            std::vector<int> tour_;
            mutation(numOfVertices, population[individualIndex],
                     tour_, method,
                     from, to);
            for (int idx = 0; idx < numOfVertices; idx++) {
                population[populationSize + 2 * hybridizationSize + i][idx] = tour_[idx];
            }
        }

        std::vector<float> currentCosts;
        selection(numOfVertices, populationSize, hybridizationSize, mutationSize,
                  costMatrix, population, currentCosts);
        cost = currentCosts[0];
    }
    for (int i = 0; i < numOfVertices; i++) {
        tour.push_back(population[0][i]);
    }
}

void initialization(int numOfVertices, int populationSize, int hybridizationSize, int mutationSize, std::vector<std::vector<int>>& population) {
    for (int i = 0; i < populationSize + hybridizationSize * 2 + mutationSize; i++) {
        population[i] = std::vector<int>(numOfVertices);
        std::vector<int> flag = std::vector<int>(numOfVertices, 0);
        for (int j = 0; j < numOfVertices; j++) {
            int ShiftingVertex = rand() % (numOfVertices - j);

            int currentVertex = 0;
            while (flag[currentVertex] != 0) {
                currentVertex++;
            }

            while (ShiftingVertex != 0) {
                currentVertex++;
                if (flag[currentVertex] == 0) {
                    ShiftingVertex--;
                }
            }

            population[i][j] = currentVertex;
            flag[currentVertex] = 1;
        }
    }
}

void hybridization(int numOfVertices, std::vector<int> tour1, std::vector<int> tour2, std::vector<int>& tour1_, std::vector<int>& tour2_, int from, int to) {
    tour1_ = std::vector<int>(numOfVertices);
    tour2_ = std::vector<int>(numOfVertices);
    std::vector<int> flag = std::vector<int>(numOfVertices, 0);
    std::vector<int> checkTour1 = std::vector<int>(numOfVertices, 0);
    std::vector<int> checkTour2 = std::vector<int>(numOfVertices, 0);

    if (from < to) {
        for (int i = from; i <= to; i++) {
            flag[i] = 1;
        }
    } else {
        for (int i = to; i <= from; i++) {
            flag[i] = 1;
        }
    }

    for (int i = 0; i < numOfVertices; i++) {
        if (flag[i] == 1) {
            tour1_[i] = tour2[i];
            tour2_[i] = tour1[i];
            for (int j = 0; j < numOfVertices; j++) {
                if (tour1[j] == tour2[i]) {
                    checkTour1[j] = 1;
                }
                if (tour2[j] == tour1[i]) {
                    checkTour2[j] = 1;
                }
            }
        }
    }

    int currentIndex1 = 0;
    int currentIndex2 = 0;
    for (int i = 0; i < numOfVertices; i++) {
        while (checkTour1[currentIndex1] == 1) {
            currentIndex1++;
        }
        while (checkTour2[currentIndex2] == 1) {
            currentIndex2++;
        }
        if (flag[i] == 0) {
            tour1_[i] = tour1[currentIndex1];
            tour2_[i] = tour2[currentIndex2];
            currentIndex1++;
            currentIndex2++;
        }
    }
}

void mutation(int numOfVertices, std::vector<int> tour, std::vector<int>& tour_, int method, int from, int to) {
    tour_ = std::vector<int>(numOfVertices);

    if (from > to) {
        int temp = to;
        to = from;
        from = temp;
    }

    if (method == 0) {
        for (int i = 0; i < numOfVertices; i++) {
            tour_[i] = tour[i];
        }
        tour_[from] = tour[to];
        tour_[to] = tour[from];
    } else if (method == 1) {
        for (int i = 0; i < from; i++) {
            tour_[i] = tour[i];
        }
        for (int i = from; i <= to; i++) {
            tour_[i] = tour[from + to - i];
        }
        for (int i = to + 1; i < numOfVertices; i++) {
            tour_[i] = tour[i];
        }
    } else {
        for (int i = 0; i < from; i++) {
            tour_[i] = tour[i];
        }
        for (int i = from; i < from + (numOfVertices - (to + 1)); i++) {
            tour_[i] = tour[i + to - from + 1];
        }
        for (int i = from + (numOfVertices - (to + 1)); i < numOfVertices; i++) {
            tour_[i] = tour[i - (numOfVertices - (to + 1))];
        }
    }
}

void selection(int numOfVertices, int populationSize, int hybridizationSize, int mutationSize, std::vector<std::vector<float>> costMatrix, std::vector<std::vector<int>>& population, std::vector<float>& currentCosts) {
    int currentPopulationSize = populationSize + 2 * hybridizationSize +
                                mutationSize;
    currentCosts = std::vector<float>(currentPopulationSize);
    for (int i = 0; i < currentPopulationSize; i++) {
        float currentCost = 0;
        for (int j = 0; j < numOfVertices; j++) {
            currentCost += costMatrix[population[i][j]][population[i][(j + 1) % numOfVertices]];
        }
        currentCosts[i] = currentCost;
    }

    for (int i = 0; i < currentPopulationSize; i++) {
        for (int j = i + 1; j < currentPopulationSize; j++) {
            if (currentCosts[i] > currentCosts[j]) {
                float tempCost = currentCosts[i];
                currentCosts[i] = currentCosts[j];
                currentCosts[j] = tempCost;
                for (int idx = 0; idx < numOfVertices; idx++) {
                    int tmp = population[i][idx];
                    population[i][idx] = population[j][idx];
                    population[j][idx] = tmp;
                }
            }
        }
    }
}
