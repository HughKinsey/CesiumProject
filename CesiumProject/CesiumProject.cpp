#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <queue>
#include <limits>
#include <cstdio>
#include <string>

const int WIDTH = 512;
const int HEIGHT = 512;
const float SPATIAL_RESOLUTION = 30.0f; // meters per pixel
const float HEIGHT_RESOLUTION = 11.0f;  // meters per height value

typedef unsigned char uint8;

typedef std::pair<float, std::pair<int, int>> Node; // {distance value, {x,y}}


//Load the heightmap from disk and store it in a 2D array.
bool loadHeightMap(const std::string& filename, std::vector<std::vector<uint8>>& heightMap) 
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) 
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return false;
    }

    heightMap.resize(HEIGHT, std::vector<uint8>(WIDTH));
    for (int y = 0; y < HEIGHT; ++y) 
    {
        file.read(reinterpret_cast<char*>(heightMap[y].data()), WIDTH);
        if (!file) 
        {
            std::cerr << "Error: Failed to read data from file." << std::endl;
            return false;
        }
    }

    file.close();
    return true;
}


float computeSurfaceDistance(const std::vector<std::vector<uint8>>& heightMap, std::pair<int, int> start, std::pair<int, int> destination)
{
    // Initialize a 2D vector to store the shortest distance to each point on the grid.
    // Set all distances to the maximum possible value initially.
    std::vector<std::vector<float>> distances(HEIGHT, std::vector<float>(WIDTH, std::numeric_limits<float>::max()));

    // Priority queue to store the points in the order of the shortest distance (min-heap).
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pointsToExplore;

    // Directions for neighboring cells (left, right, up, down, and diagonals).
    int dx[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };
    int dy[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };

    // Start with the start point, setting its distance to 0.
    pointsToExplore.push({ 0.0f, start });
    distances[start.second][start.first] = 0.0f;

    // While there are still points to explore.
    while (!pointsToExplore.empty()) 
    {
        // Get the point with the current shortest distance.
        Node currentNode = pointsToExplore.top();
        pointsToExplore.pop();

        float currentDistance = currentNode.first;
        std::pair<int, int> currentPoint = currentNode.second;
        int x = currentPoint.first, y = currentPoint.second;

        // If we've reached the destination, return the current distance.
        if (x == destination.first && y == destination.second) 
        {
            return currentDistance;
        }

        // Explore the 8 neighboring points (up, down, left, right, and diagonals).
        for (int i = 0; i < 8; ++i) 
        {
            int neighborX = x + dx[i];
            int neighborY = y + dy[i];

            // Ensure the neighboring point is within the bounds of the height map.
            if (neighborX >= 0 && neighborX < WIDTH && neighborY >= 0 && neighborY < HEIGHT) 
            {

                // Calculate the vertical distance (height difference) between the current point and the neighbor.
                float verticalDistance = (heightMap[neighborY][neighborX] - heightMap[y][x]) * HEIGHT_RESOLUTION;

                // Calculate the horizontal distance between points.
                // Diagonal movements have a greater distance (scaled by sqrt(2)).
                float horizontalDistance = (i < 4) ? SPATIAL_RESOLUTION : SPATIAL_RESOLUTION * std::sqrt(2);

                // Calculate the total distance (hypotenuse of the horizontal and vertical distance).
                float totalDistance = currentDistance + std::sqrt(horizontalDistance * horizontalDistance + verticalDistance * verticalDistance);

                // If the new total distance is shorter than the previously known distance, update it.
                if (totalDistance < distances[neighborY][neighborX]) 
                {
                    distances[neighborY][neighborX] = totalDistance;
                    // Push the neighbor into the priority queue to explore later.
                    pointsToExplore.push({ totalDistance, {neighborX, neighborY} });
                }
            }
        }
    }

    // If no valid path was found, return -1.
    return -1.0f;
}



int main() 
{

    std::string Prefilename;
    std::cout << "Enter the file path for Pre Data: ";
    std::getline(std::cin, Prefilename);

    std::string Postfilename;
    std::cout << "Enter the file path for Post Data: ";
    std::getline(std::cin, Postfilename);

    // User inputs for Point A
    int ax, ay;
    std::cout << "Enter coordinates for Point A (x y): ";
    std::cin >> ax >> ay;
    std::pair<int, int> PointA = { ax, ay };

    // User inputs for Point B
    int bx, by;
    std::cout << "Enter coordinates for Point B (x y): ";
    std::cin >> bx >> by;
    std::pair<int, int> PointB = { bx, by };


    std::cout << "Pre File: " << Prefilename << std::endl;
    std::cout << "Post File: " << Postfilename << std::endl;
    std::cout << "Point A: (" << PointA.first << ", " << PointA.second << ")" << std::endl;
    std::cout << "Point B: (" << PointB.first << ", " << PointB.second << ")" << std::endl;


    //Handle Pre Eruption Calculations
    std::vector<std::vector<uint8>> heightMap;
    float distancePreEruption = 0;

    if (loadHeightMap(Prefilename, heightMap))
    {
        distancePreEruption = computeSurfaceDistance(heightMap, PointA, PointB);
        if (distancePreEruption > 0)
        {
            std::cout << "Surface distance Pre Eruption from A to B: " << distancePreEruption << " meters" << std::endl;
        }
        else 
        {
            std::cout << "No valid path found." << std::endl;
        }
    }



    ////Handle Post Eruption Calculations
    std::vector<std::vector<uint8>> PostheightMap;
    float distancePostEruption = 0;
    if (loadHeightMap(Postfilename, PostheightMap)) 
    {
        distancePostEruption = computeSurfaceDistance(PostheightMap, PointA, PointB);
        if (distancePostEruption > 0) 
        {
            std::cout << "Surface distance from Post Eruption from A to B: " << distancePostEruption << " meters" << std::endl;
        }
        else 
        {
            std::cout << "No valid path found." << std::endl;
        }
    }

    //Find the difference between the two.
    float DistanceChange = std::abs(distancePostEruption - distancePreEruption);
    std::cout << "Distance Change:" << DistanceChange << std::endl;

    //keep the window open until a something is entered.
    int exit;
    std::cout << "Exit?: ";
    std::cin >> exit;

    std::getchar(); 
    return 0;
}
