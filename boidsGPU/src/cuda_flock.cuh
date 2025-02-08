#ifndef CUDA_FLOCK_CUH
#define CUDA_FLOCK_CUH

#include <vector>

void updateBoidsCUDA(
                     std::vector<float>& boidPosX, std::vector<float>& boidPosY,
                     std::vector<float>& boidVelX, std::vector<float>& boidVelY,
                     std::vector<float>& accelerationX, std::vector<float>& accelerationY
);

#endif