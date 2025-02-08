#ifndef VECTOR2D
#define VECTOR2D
#include <array>
#include <cmath>
#include <immintrin.h>

class Vec2D
{
  public:

    Vec2D(float x, float y)
    {
        Data[0] = x;
        Data[1] = y;
    }

    Vec2D(const std::array<float, 2> &Copy)
    {
        for (size_t i = 0; i < 2; i++)
        {
            Data[i] = Copy[i];
        }
    }

    // float SizeSqr() const
    // {
    //     __m256 vector = _mm256_set_ps(0,0,0,0,0,0, Data[1], Data[0]);
    //     vector = _mm256_mul_ps(vector, vector);
    //     vector = _mm256_hadd_ps(vector, vector);

    //     // now we want to extract the resulting sum and return
    //     float result[8];
    //     _mm256_store_ps(result, vector);
    //     return result[0];
    // }

    // float Size() const
    // {
    //     __m256 vector = _mm256_set_ps(0,0,0,0,0,0, Data[1], Data[0]);
    //     vector = _mm256_mul_ps(vector, vector);
    //     vector = _mm256_hadd_ps(vector, vector);
    //     vector = _mm256_sqrt_ps(vector);

    //     // extract result of vector operations

    //     float result[8];
    //     _mm256_store_ps(result, vector);
    //     return result[0];
    // }

    // Vec2D Norm() const
    // {
    //     float size = Size();
    //     __m256 vector = _mm256_set_ps(0,0,0,0,0,0, Data[1], Data[0]);
    //     __m256 sizeVector = _mm256_set1_ps(size);

    //     vector = _mm256_div_ps(vector, sizeVector);

    //     float result[8];
    //     _mm256_store_ps(result, vector);
    //     return  Vec2D(result[0], result[1]);
        
    // }

    float SizeSqr() const
    {
        float sum = 0;
        for (size_t i = 0; i < 2; i++)
        {
            sum += Data[i] * Data[i];
        }
        return sum;
    }

    float Size() const
    {
        // much slower that sizesqr
        return std::sqrt(SizeSqr());
    }

    float FastInverseSqrt(float number) const {
        const float threehalfs = 1.5F;

        float x2 = number * 0.5F;
        float y = number;

        // "evil floating point bit level hacking" :)
        long i = * ( long * ) &y; 
        i = 0x5f3759df - ( i >> 1 ); 
        y = * ( float * ) &i; 

        // first iteration of Newton's method
        y = y * (threehalfs - (x2 * y * y));

        return y;
    }   

    Vec2D Norm() const
    {
        // could also use quake inv sqrt
        return Vec2D(Data) / Size();
        // return Vec2D(Data) * FastInverseSqrt(SizeSqr());
    }

    Vec2D operator+(const Vec2D &other) const
    {
        Vec2D Returner(Data);
        for (size_t i = 0; i < 2; i++)
        {
            Returner.Data[i] += other.Data[i];
        }
        return Returner;
    }

    Vec2D operator-(const Vec2D &other) const
    {
        Vec2D Returner(Data);
        for (size_t i = 0; i < 2; i++)
        {
            Returner.Data[i] -= other.Data[i];
        }
        return Returner;
    }

    Vec2D operator/(const float divisor) const
    {
        Vec2D Returner(Data);
        for (size_t i = 0; i < 2; i++)
        {
            Returner.Data[i] /= divisor;
        }
        return Returner;
    }

    // Vec2D operator/(const float divisor) const
    // {
    //     // Load vector and divisor into AVX registers
    //     __m256 vec = _mm256_set_ps(0, 0, 0, 0, 0, 0, Data[1], Data[0]);
    //     __m256 div = _mm256_set1_ps(divisor);
        
    //     // Perform division
    //     __m256 result = _mm256_div_ps(vec, div);
        
    //     // Extract results
    //     float resultArray[8];
    //     _mm256_store_ps(resultArray, result);
        
    //     return Vec2D(resultArray[0], resultArray[1]);
    // }

    Vec2D operator*(const float scalar) const
    {
        Vec2D Returner(Data);
        for (size_t i = 0; i < 2; i++)
        {
            Returner.Data[i] *= scalar;
        }
        return Returner;
    }
    
    void operator+=(const Vec2D &other)
    {
        for (size_t i = 0; i < 2; i++)
        {
            Data[i] += other.Data[i];
        }
    }

    void operator-=(const Vec2D &other)
    {
        for (size_t i = 0; i < 2; i++)
        {
            Data[i] -= other.Data[i];
        }
    }

    void operator/=(const float divisor)
    {
        for (size_t i = 0; i < 2; i++)
        {
            Data[i] /= divisor;
        }
    }

    void operator*=(const float scalar)
    {
        for (size_t i = 0; i < 2; i++)
        {
            Data[i] *= scalar;
        }
    }

    void operator=(const Vec2D &other)
    {
        for (size_t i = 0; i < 2; i++)
        {
            // ensures elements are present
            Data[i] = other.Data[i];
        }
    }

    float operator[](const size_t i) const
    {
        return Data[i];
    }

    // non const setter
    float& operator[](const size_t i)
    {
        return Data[i];
    }

    std::array<float, 2> Data; 
};

#endif