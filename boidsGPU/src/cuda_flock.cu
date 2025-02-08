
#include "cuda_flock.cuh"

__global__ void updateBoidsKernel(float* boidPosX, float* boidPosY,
                                  float* boidVelX, float* boidVelY,
                                  float* accelerationX, float* accelerationY,
                                  int numBoids) 
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (i < numBoids) {
        // init constants
        float cohesion = 0.3;
        float alignment = 0.8;
        float separation = 3.0;
        float visionRadius = 5.0;
        float sqrVisionRadius = visionRadius * visionRadius;
        float maxVel = 1.0;
        float maxAccel = 0.1;
        // init arrays
        float cohesionForceX = 0.0, cohesionForceY = 0.0;
        float alignmentForceX = 0.0, alignmentForceY = 0.0;
        float separationForceX = 0.0, separationForceY = 0.0;
        float avgVelocityX = 0.0, avgVelocityY = 0.0;
        int neighbourCount = 0;
        float posX = boidPosX[i];
        float posY = boidPosY[i];
        float velX = boidVelX[i];
        float velY = boidVelY[i];
        for (int j = 0; j < numBoids; ++j) {
            float otherPosX = boidPosX[j];
            float otherPosY = boidPosY[j];
            
            float diffX = otherPosX - posX;
            float diffY = otherPosY - posY;
            float sqrDist = diffX * diffX + diffY * diffY;
            if (sqrDist < sqrVisionRadius) {
                cohesionForceX += diffX;
                cohesionForceY += diffY;
                avgVelocityX += boidVelX[j];
                avgVelocityY += boidVelY[j];

                if (sqrDist > 0) { 
                    float dist = sqrtf(sqrDist);
                    separationForceX += diffX / dist;
                    separationForceY += diffY / dist;
                }
                
                neighbourCount++;
            }
        }
        if (neighbourCount > 0) {
            // calculate cohesion
            cohesionForceX = (cohesionForceX / neighbourCount) * cohesion;
            cohesionForceY = (cohesionForceY / neighbourCount) * cohesion;

            // calculate alignment
            avgVelocityX /= neighbourCount;
            avgVelocityY /= neighbourCount;
            float avgVelMagnitude = sqrtf(avgVelocityX * avgVelocityX + avgVelocityY * avgVelocityY);
            if (avgVelMagnitude > 0) {
                float desiredVelX = (avgVelocityX / avgVelMagnitude) * maxVel;
                float desiredVelY = (avgVelocityY / avgVelMagnitude) * maxVel;
                alignmentForceX = (desiredVelX - velX) * alignment;
                alignmentForceY = (desiredVelY - velY) * alignment;
            }
            // calculate separation
            float sepMag = sqrtf(separationForceX * separationForceX + separationForceY * separationForceY);
            if (sepMag > 0) {
                separationForceX = (separationForceX / sepMag) * separation * -1.0;
                separationForceY = (separationForceY / sepMag) * separation * -1.0;
            }
            // limit forces
            float cohMag = sqrtf(cohesionForceX * cohesionForceX + cohesionForceY * cohesionForceY);
            if (cohMag > maxAccel) {
                cohesionForceX = (cohesionForceX / cohMag) * maxAccel;
                cohesionForceY = (cohesionForceY / cohMag) * maxAccel;
            }
            float alignMag = sqrtf(alignmentForceX * alignmentForceX + alignmentForceY * alignmentForceY);
            if (alignMag > maxAccel) {
                alignmentForceX = (alignmentForceX / alignMag) * maxAccel;
                alignmentForceY = (alignmentForceY / alignMag) * maxAccel;
            }
            
            if (sepMag > maxAccel) {
                separationForceX = (separationForceX / sepMag) * maxAccel;
                separationForceY = (separationForceY / sepMag) * maxAccel;
            }
            accelerationX[i] = cohesionForceX + alignmentForceX + separationForceX;
            accelerationY[i] = cohesionForceY + alignmentForceY + separationForceY;
        } else {
            accelerationX[i] = 0.0;
            accelerationY[i] = 0.0;
        }
    }
}
void updateBoidsCUDA(std::vector<float>& boidPosX, std::vector<float>& boidPosY,
                     std::vector<float>& boidVelX, std::vector<float>& boidVelY,
                     std::vector<float>& accelerationX, std::vector<float>& accelerationY) 
{
    int numBoids = boidPosX.size();
    // device pointers
    float *d_boidPosX, *d_boidPosY, *d_boidVelX, *d_boidVelY, *d_accelerationX, *d_accelerationY;
    // allocate device memory
    cudaMalloc((void**)&d_boidPosX, numBoids * sizeof(float));
    cudaMalloc((void**)&d_boidPosY, numBoids * sizeof(float));
    cudaMalloc((void**)&d_boidVelX, numBoids * sizeof(float));
    cudaMalloc((void**)&d_boidVelY, numBoids * sizeof(float));
    cudaMalloc((void**)&d_accelerationX, numBoids * sizeof(float));
    cudaMalloc((void**)&d_accelerationY, numBoids * sizeof(float));
    // cpu --> gpu
    cudaMemcpy(d_boidPosX, boidPosX.data(), numBoids * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_boidPosY, boidPosY.data(), numBoids * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_boidVelX, boidVelX.data(), numBoids * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_boidVelY, boidVelY.data(), numBoids * sizeof(float), cudaMemcpyHostToDevice);
    // run kernel func
    // 256 works fine
    int threadsPerBlock = 256;
    int blocksPerGrid = (numBoids + threadsPerBlock - 1) / threadsPerBlock;
    updateBoidsKernel<<<blocksPerGrid, threadsPerBlock>>>(d_boidPosX, d_boidPosY, 
                                                          d_boidVelX, d_boidVelY,
                                                          d_accelerationX, d_accelerationY, 
                                                          numBoids);

    // get acceleration updates back to cpu
    cudaMemcpy(accelerationX.data(), d_accelerationX, numBoids * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(accelerationY.data(), d_accelerationY, numBoids * sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_boidPosX);
    cudaFree(d_boidPosY);
    cudaFree(d_boidVelX);
    cudaFree(d_boidVelY);
}
