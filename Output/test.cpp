#include <math.h>
#include <random>
#include <thread>
#if defined(_MSC_VER)
#   include <xmmintrin.h>
#else
#   include <x86intrin.h>
#endif

#define M128RAND _mm_set_ps(0.0f, RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f))
#define FLOATRAND RandomRange(100.0f, 1000.0f)

float RandomRange(float low, float high);

void TestA() {
    // Test 0: SSE Dot Product
    // Domain: xo-math
    // Sub-Domain: Dot Product
    // Technology: SSE

    // Setup:
    __m128 a = M128RAND;
    __m128 b = M128RAND;
    __m128 mask = _mm_castsi128_ps(_mm_set_epi32(0, 0xffffffff, 0xffffffff, 0xffffffff));
    float dot;

    // Run:
    __m128 temp = _mm_and_ps(_mm_mul_ps(a, b), mask);
    temp = _mm_hadd_ps(temp, temp);
    temp = _mm_hadd_ps(temp, temp);
    dot = _mm_cvtss_f32(temp);

}

void TestB() {
    // Test 1: Float Dot Product
    // Domain: none
    // Sub-Domain: Dot Product
    // Technology: 

    // Setup:
    float v1x = FLOATRAND;
    float v1y = FLOATRAND;
    float v1z = FLOATRAND;
    float v2x = FLOATRAND;
    float v2y = FLOATRAND;
    float v2z = FLOATRAND;
    float dot;

    // Run:
    dot = (v1x*v2x) + (v1y*v2y) + (v1y*v2y);

}


int main() {
	TestA();
	TestB();
}

float RandomRange(float low, float high) {
    static thread_local std::mt19937 engine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> dist(low, high);
    return dist(engine);
}
