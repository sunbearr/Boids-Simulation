#include "grid.hpp"
#include <vector>
#include <unordered_set>
#include <map>

Grid::Grid(const Vec2D dimensions, const int cellLength):
dimensions(dimensions), cellLength(cellLength) {}

void Grid::addBoid(Boid* boid) {
    int key = computeKey(boid->getPosition());
    boid_map[key].insert(boid);
}

void Grid::updateBoid(Boid* boid) {
    int key = computeKey(boid->getPosition());
    // don't update if the boid hasn't moved to a new cell
    if (boid->gridKey != key) {
        boid_map[boid->gridKey].erase(boid);
        boid_map[key].insert(boid);
        boid->gridKey = key;
    }
}

int Grid::computeKey(Vec2D position) {
    // map position to xy cell number
    int x = floor(position[0] / cellLength);
    int y = floor(position[1] / cellLength);

    // compute unique key for each grid
    return x + y * dimensions[0];
}

std::vector<Boid*> Grid::getNeighbours(Boid* boid) {
    std::vector<Boid*> neighbours;
    Vec2D position = boid->getPosition();
    
    // get range of grid cells to check based on the vision radius
    int range = ceil(boid->visionRadius / cellLength);

    int x = floor(position[0] / cellLength);
    int y = floor(position[1] / cellLength);

    // check current cell and surrounding cells for neighbours.
    for (int dx = -range; dx <= range; ++dx) {
        for (int dy = -range; dy <= range; ++dy) {
            int neighborKey = (x + dx) + (y + dy) * dimensions[0];
            
            // check if the key exists in the map
            if (boid_map.find(neighborKey) != boid_map.end()) {
                for (Boid* neighbor : boid_map[neighborKey]) {
                    // add potential neighbour to neighbours
                    neighbours.push_back(neighbor);
                }
            }
        }
    }

    return neighbours;
}

