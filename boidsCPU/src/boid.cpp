#include <iostream>
#include <vector>
#include "boid.hpp"
using namespace std;

// fixed boid params
float Boid::cohesion = 0.3;
float Boid::alignment = 0.8;
float Boid::separation = 3;
float Boid::visionRadius = 5;
float Boid::sqrVisionRaidus = 5*5;
float Boid::maxVel = 1;
float Boid::maxAccel = 0.1;

Boid::Boid(const Vec2D& position, const Vec2D& velocity, const Vec2D& acceleration):
 position_(position), velocity_(velocity), acceleration_(acceleration) {}

// used to update the position and velocity of the boid
void Boid::Update() {

    // keep velocity within maximum
    float speed = velocity_.Size();
    if (speed > maxVel) {
        velocity_ = velocity_.Norm() * maxVel;
    }

    position_ += velocity_;
    velocity_ += acceleration_;
    

    // keep boid in bounds
    wrapAround();

    // reset acceleration each time step
    acceleration_ *= 0;

}

// if boid position is out of bounds, wrap around to other side
void Boid::wrapAround() {
    if (position_[0] > 100){
        position_[0] = 0;
    }
    if (position_[0] < 0){
        position_[0] = 100;
    }
    if (position_[1] > 100){
        position_[1] = 0;
        
    }
    if (position_[1] < 0){
        position_[1] = 100;
    }
}

Vec2D Boid::getPosition() const {
    return position_;
}

Vec2D Boid::getVelocity(){
    return velocity_;
}

void Boid::Cohere(const vector<Boid*>& boids) {
    // direction towards centre of mass of nearby boids
    Vec2D comDir = Vec2D(0,0);
    int neighbourCount = 0;
    // loop through each boid, if within vision radius contribute
    // to COM. We want to move towards the average pos of boids
    // within vision radius.
    for (const Boid* boid : boids) {
        Vec2D otherPos = boid->getPosition();
        Vec2D diff = otherPos - position_;

        if (diff.Size() < visionRadius) {
            comDir += diff;
            neighbourCount++;
        }
    }

    if (neighbourCount > 0) {
        // scale comDir to number of neighbours
        comDir /= neighbourCount;
        // scale the steering force to cohesion factor
        Vec2D force = comDir * cohesion;

        // limit steering force to the maximum acceleration
        float forceMag = force.Size();
        if (forceMag > maxAccel) {
            force = force.Norm() * maxAccel;
        }
        
        // when combining these, this will be additive.
        acceleration_ += force;
    }
}

void Boid::Separate(const vector<Boid*>& boids) {

    Vec2D totalSeparation = Vec2D(0,0);

    for (Boid* boid : boids) {

        if (boid == this) continue;

        Vec2D otherPos = boid->getPosition();
        Vec2D diff = position_ - otherPos;

        if (diff.Size() > 0 && diff.Size() < visionRadius) {
            // add weighted separation to total
            totalSeparation += diff.Norm() / diff.Size();
            
        }
    }

    if (totalSeparation.Size() > 0) {
        totalSeparation = totalSeparation.Norm() * separation;

        // scale total separation to separation coefficient
        totalSeparation *= separation;

        float forceMag = totalSeparation.Size();
        if (forceMag > maxAccel) {
            totalSeparation = totalSeparation.Norm() * maxAccel;
        }

        acceleration_ += totalSeparation;
    }

}

void Boid::Align(const vector<Boid*>& boids) {
    Vec2D avgVelocity = Vec2D(0,0);
    int neighbourCount = 0;

    for (Boid* boid : boids) {

        if (boid == this) continue;

        Vec2D diff = boid->getPosition() - position_;
        float distance = diff.Size();

        if (distance > 0 && distance < visionRadius) {
            // add weighted separation to total
            avgVelocity += boid->getVelocity();
            neighbourCount++;
        }
    }

    if (neighbourCount > 0 && avgVelocity.Size() > 0) {

        avgVelocity /= neighbourCount;
        
        Vec2D desiredVelocity = avgVelocity.Norm() * maxVel;
        Vec2D force = desiredVelocity - velocity_;

        force *= alignment;

        if (force.Size() > maxAccel) {
            force = force.Norm() * maxAccel;
        }

        acceleration_ += force;
        
    }

    
}

float Boid::DistTo(Boid& boid) {
    // euclidian distance to other boid
    return sqrt(pow(position_[0] - boid.getPosition()[0], 2) + pow(position_[1] - boid.getPosition()[1], 2));
}   

void Boid::UpdateBehaviour(const vector<Boid*>& boids) {
    // force vectors
    Vec2D cohesionForce = Vec2D(0,0);
    Vec2D alignmentForce = Vec2D(0,0);
    Vec2D separationForce = Vec2D(0,0);
    Vec2D avgVelocity = Vec2D(0,0);
    int neighbourCount = 0;

    // loop through each boid and calculate values needed for force calculations
    for (const Boid* boid : boids) {
        Vec2D otherPos = boid->getPosition();
        Vec2D diff = otherPos - position_;
        float sqrDist = diff.SizeSqr();

        // use squared values to avoid expensive sqrt
        if (sqrDist < sqrVisionRaidus) {

            //cohesion
            cohesionForce += diff;

            //alignment
            avgVelocity += boid->velocity_;

            // separation
            if (sqrDist > 0) {
                separationForce += diff.Norm() / sqrt(sqrDist);
            }
        
            neighbourCount++;
        }
    }

    if (neighbourCount > 0) {
        // scale cohesion to number of neighbours
        cohesionForce /= neighbourCount;
        // scale cohesion to cohesion factor
        cohesionForce *= cohesion;

        // calculate alignment
        if (avgVelocity.SizeSqr() > 0) {
            avgVelocity /= neighbourCount;
            Vec2D desiredVelocity = avgVelocity.Norm() * maxVel;
            alignmentForce = (desiredVelocity - velocity_) * alignment;
        }

        // calculate separation
        if (separationForce.SizeSqr() > 0) {
            separationForce = (separationForce.Norm() * separation) * -1;
        }
    

        // limit steering forces to the maximum acceleration
        
        if (cohesionForce.SizeSqr() > maxAccel * maxAccel) {
            cohesionForce = cohesionForce.Norm() * maxAccel;
        }

        if (alignmentForce.SizeSqr() > maxAccel * maxAccel) {
            alignmentForce = alignmentForce.Norm() * maxAccel;
        }

        if (separationForce.SizeSqr() > maxAccel * maxAccel) {
            separationForce = separationForce.Norm() * maxAccel;
        }
        
        acceleration_ += cohesionForce + alignmentForce + separationForce;

    }

}



