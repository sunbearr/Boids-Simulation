#include "flock.hpp"
#include "boid.hpp"
#include "grid.hpp"
#include "cuda_flock.cuh"
#include <iostream>
#include <unordered_map>

//20x20 grid
Flock::Flock(): grid(Vec2D(100,100), 5) {
    // Any other initialization if needed
}

void Flock::performFLockingCUDA(){
    // init pos and vel arrays to send to gpu
    std::vector<float> boidPositionsX(flock.size());
    std::vector<float> boidPositionsY(flock.size());
    std::vector<float> boidVelocitiesX(flock.size());
    std::vector<float> boidVelocitiesY(flock.size());

    // get full list of boid neighbours (no spatial hashing).
    for (size_t i = 0; i < flock.size(); ++i) {
        // do boid updates
        Boid& boid = flock[i];
        boid.Update();

        // store boid positions and velocities
        boidPositionsX[i] = boid.position_[0];
        boidPositionsY[i] = boid.position_[1];
        boidVelocitiesX[i] = boid.velocity_[0];
        boidVelocitiesY[i] = boid.velocity_[1];

    }

    std::vector<float> accelerationX(flock.size());
    std::vector<float> accelerationY(flock.size());

    updateBoidsCUDA(boidPositionsX, boidPositionsY,
                    boidVelocitiesX, boidVelocitiesY,
                    accelerationX, accelerationY);
    
    // now we have updated accelerations, update boid acceleration
    for (size_t i = 0; i < flock.size(); ++i) {
        Boid& boid = flock[i];
        boid.acceleration_ = Vec2D(accelerationX[i], accelerationY[i]);
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

