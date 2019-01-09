#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void generateVertices(std::string fileName, int height, int width, int stepSize, int border, int numOfVertices, std::vector<std::vector<int>>& vertices);
void computeCostMatrix(std::string fileName, int numOfVertices, std::vector<std::vector<int>> vertices, std::vector<std::vector<float>>& costMatrix);
void greedy(int numOfVertices, int start, std::vector<std::vector<float>> costMatrix, std::vector<int>& tour, float& cost);

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

    int start = 0;
    std::vector<int> greedyTour;
    float greedyCost;
    greedy(numOfVertices, start, costMatrix, greedyTour, greedyCost);
    cv::Mat greedyMap(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < numOfVertices; i++) {
        cv::circle(greedyMap, cv::Point(vertices[i][0], vertices[i][1]),
                   1, cv::Scalar(0, 0, 255), 2, 0);
        cv::imshow("Cities", greedyMap);
        cv::waitKey(1);
    }
    for (int i = 0; i < numOfVertices - 1; i++) {
        cv::line(greedyMap,
                 cv::Point(vertices[greedyTour[i]][0], vertices[greedyTour[i]][1]),
                 cv::Point(vertices[greedyTour[i + 1]][0], vertices[greedyTour[i + 1]][1]),
                 cv::Scalar(255, 0, 0), 1, 8, 0);
        cv::imshow("Greedy", greedyMap);
        cv::waitKey(1);
    }
    std::string greedyText = "numOfVertices = ";
    greedyText.append(std::to_string(numOfVertices));
    greedyText.append("     greedy: ");
    greedyText.append(std::to_string(greedyCost));
    cv::putText(greedyMap, greedyText, cv::Point(border, height - border),
                cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 8);
    cv::imshow("Greedy", greedyMap);
    cv::imwrite("greedy.png", greedyMap);
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

void greedy(int numOfVertices, int start, std::vector<std::vector<float>> costMatrix, std::vector<int>& tour, float& cost) {
    std::vector<int> flag = std::vector<int>(numOfVertices, 0);
    tour = std::vector<int>(numOfVertices);
    tour[0] = start;
    flag[start] = 1;
    int currentVertex = start;
    cost = 0;
    for (int step = 1; step < numOfVertices; step++) {
        float minCost = 9999;
        int nearestVertex;
        for (int nextVertex = 0; nextVertex < numOfVertices; nextVertex++) {
            if (minCost > costMatrix[currentVertex][nextVertex] &&
                flag[nextVertex] == 0) {
                minCost = costMatrix[currentVertex][nextVertex];
                nearestVertex = nextVertex;
            }
        }
        tour[step] = nearestVertex;
        flag[nearestVertex] = 1;
        currentVertex = nearestVertex;
        cost += minCost;
    }
    cost += costMatrix[tour[numOfVertices - 1]][start];
}
