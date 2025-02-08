#include <iostream>
#include <random>
#include <fstream>
#include <ctime>
#include <omp.h>
#include "flock.hpp"
#include "boid.hpp"
#include "Vec.hpp"

class Simulator {
    public:
        int numBoids = 1000;
        int timeSteps = 600;
        std::ofstream outputFile{"boid_positions.txt"};
        Flock flock;
        Simulator() {}

        void populateFlock() {

            std::random_device rd;
            std::mt19937 gen(rd());

            // seed for consistent init
            unsigned int seed = 42;
            gen.seed(42);

            std::uniform_real_distribution<> posDistrib(0.0, 100.0);
            std::uniform_real_distribution<> velDistrib(-2.0, 2.0);
            
            for (int i = 0; i < numBoids; i++) {
                // this way of doing rng is slow but only done at start
                float x = posDistrib(gen);
                float y = posDistrib(gen);
                float vx = velDistrib(gen);
                float vy = velDistrib(gen);
               // Boid boid(Vec2D(x, y), Vec2D((vx-0.5)/100,(vy-0.5)/100), Vec2D(0.1,0.1));
                Boid boid(Vec2D(x, y), Vec2D(vx,vy), Vec2D(0,0));
                flock.addBoid(boid);
            }
        }

        void simulate() {
            populateFlock();
            for (int t = 0; t < timeSteps; t++) {

                outputFile << "Timestep " << t << ":\n";
                for (const Boid& boid : flock.getBoids()) {
                    outputFile << boid.position_[0] << " " << boid.position_[1] << "\n";
                }
                outputFile << "\n"; 

                flock.performFLockingCUDA();
            }
        }
};

int main(int argc, char const *argv[])
{

    #pragma omp parallel
    {
        #pragma omp single
        {
            std::cout << "Number of threads: " << omp_get_num_threads() << std::endl;
        }
        
        #pragma omp critical
        {
            std::cout << "Thread " << omp_get_thread_num() << " is running." << std::endl;
        }
    }
    

    clock_t startTime = clock();
    Simulator sim;
    std::cout << "Number of boids is " << sim.numBoids << " boids." << std::endl;
    std::cout << "Number of timesteps is " << sim.timeSteps << "" << std::endl;
    //sim.populateFlock();
    sim.simulate();

     // Stop the clock
    clock_t endTime = clock();

    // Calculate the elapsed time in seconds
    double elapsedSeconds = double(endTime - startTime) / CLOCKS_PER_SEC;

    // Output the result
    std::cout << "Time taken: " << elapsedSeconds << " seconds" << std::endl;
    return 0;
}