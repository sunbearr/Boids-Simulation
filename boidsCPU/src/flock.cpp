#include "flock.hpp"
#include "boid.hpp"
#include "grid.hpp"
#include <iostream>
#include <unordered_map>

// 20x20 grid
Flock::Flock(): grid(Vec2D(100,100), 5) {
    // Any other initialization if needed
}

void Flock::performFLocking(){
    // cache for storing shared neighbouring boids
    std::unordered_map<int, std::vector<Boid*>> neighboursCache;
    for (Boid& boid : flock) {
         boid.Update();

         int key = boid.gridKey;

        // check if the neighbours for this gridKey are already computed
        if (neighboursCache.find(key) == neighboursCache.end()) {
            // if not in the cache, compute the neighbours and store them
            neighboursCache[key] = grid.getNeighbours(&boid);
        }

        // use the precomputed neighbours
        std::vector<Boid*> neighbours = neighboursCache[key];

         boid.UpdateBehaviour(neighbours);

        // update the grid based on the boids new position
        grid.updateBoid(&boid);
        
    }
}

void Flock::addBoid(Boid boid) {
    flock.push_back(std::move(boid));
    // ensure boid added to grid is the same as being added to flock
    grid.addBoid(&boid);

}

Boid& Flock::getBoid(size_t i) {
    return flock[i];
}

const vector<Boid>& Flock::getBoids() {
    return flock;
}

