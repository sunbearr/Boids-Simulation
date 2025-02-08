#ifndef GRID_HPP
#define GRID_HPP

#include "boid.hpp"
#include "Vec.hpp"
#include <vector>
#include <unordered_set>
#include <map>

class Grid {

    private:
        int cellLength;
        Vec2D dimensions;
        std::map<int, std::unordered_set<Boid*>> boid_map;


    public:
        Grid(const Vec2D dimensions, const int cellLength);
        int computeKey(Vec2D position);

        void addBoid(Boid* boid);
        void updateBoid(Boid* boid);
        std::vector<Boid*> getNeighbours(Boid* boid);

};

#endif // GRID_HPP