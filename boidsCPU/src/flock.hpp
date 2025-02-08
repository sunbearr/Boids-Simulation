#include <iostream>
#include <vector>
#include "boid.hpp"
#include "grid.hpp"


class Flock {
    public:
        // construct flock 
        Flock();
        // perform updates on all boids in flock
        void performFLocking();
        // get boid at some flock index
        Boid &getBoid(size_t i);
        // add boid to flock
        void addBoid(Boid boid);

        const vector<Boid>& getBoids();

    private:
        vector<Boid> flock;
        Grid grid;
        
};