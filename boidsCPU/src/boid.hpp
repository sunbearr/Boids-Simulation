#ifndef BOID_HPP
#define BOID_HPP

#include <iostream> 
#include <vector> 
#include "Vec.hpp"
using namespace std; 

class Boid {
    /**
     * 
     */
    public:
        static float cohesion;
        static float alignment;
        static float separation;
        static float visionRadius;
        static float sqrVisionRaidus;
        static float maxVel;
        static float maxAccel;
        

        Vec2D position_;
        Vec2D velocity_;
        Vec2D acceleration_;
        int gridKey;

        Boid(const Vec2D& position, const Vec2D& velocity, const Vec2D& acceleration);

        void Update();
        void Move();
        void wrapAround();
        void Cohere(const vector<Boid*>& boids);
        void Align(const vector<Boid*>& boids);
        void Separate(const vector<Boid*>& boids);
        // calculate euclidian distance to another boid
        void UpdateBehaviour(const vector<Boid*>& boids);
        float DistTo(Boid& boid);
        Vec2D getPosition() const;
        Vec2D getVelocity();

};

#endif // BOID_HPP