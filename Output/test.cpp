#include <math.h>
#include <random>
#include <thread>
#include <iostream>
#if defined(_MSC_VER)
#   include <xmmintrin.h>
#else
#   include <x86intrin.h>
#endif

using namespace std;

#define M128RAND _mm_set_ps(0.0f, RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f))
#define FLOATRAND RandomRange(100.0f, 1000.0f)

float RandomRange(float low, float high);

volatile bool TestStarted = false;
volatile bool TestFinished = false;

#pragma clang optimize off

volatile unsigned long long TestACount = 0;
void TestA() {
    // Test 0: SSE Dot Product
    // Domain: xo-math
    // Sub-Domain: Dot Product
    // Technology: SSE

    ////////// Setup (generated):
    __m128 a = M128RAND;
    __m128 b = M128RAND;
    __m128 mask = _mm_castsi128_ps(_mm_set_epi32(0, 0xffffffff, 0xffffffff, 0xffffffff));
    float dot;
    ////////// End Setup (generated):
    while(TestStarted == false) { }
    while(TestFinished == false) {
        ////////// Run (generated):
        __m128 temp = _mm_and_ps(_mm_mul_ps(a, b), mask);
        temp = _mm_hadd_ps(temp, temp);
        temp = _mm_hadd_ps(temp, temp);
        dot = _mm_cvtss_f32(temp);
        ////////// End Run (generated):
        TestACount++;
    }
}

volatile unsigned long long TestBCount = 0;
void TestB() {
    // Test 1: Float Dot Product
    // Domain: none
    // Sub-Domain: Dot Product
    // Technology: 

    ////////// Setup (generated):
    float v1x = FLOATRAND;
    float v1y = FLOATRAND;
    float v1z = FLOATRAND;
    float v2x = FLOATRAND;
    float v2y = FLOATRAND;
    float v2z = FLOATRAND;
    float dot;
    ////////// End Setup (generated):
    while(TestStarted == false) { }
    while(TestFinished == false) {
        ////////// Run (generated):
        dot = (v1x*v2x) + (v1y*v2y) + (v1y*v2y);
        ////////// End Run (generated):
        TestBCount++;
    }
}

#pragma clang optimize on

int main() {
    cout << "==========Starting test." << endl;
    
    std::thread threadA(&TestA);
    std::thread threadB(&TestB);

    int seconds = 10;
    TestStarted = true;

    for(int i = 0; i < seconds; ++i) {
        cout << "Testing... " << seconds - i << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    TestFinished = true;

    threadA.join();
    threadB.join();

    cout << "TestA: " << TestACount << endl;
    cout << "TestB: " << TestBCount << endl;

    cout << "==========Ending test." << endl;
    return 0;
}

float RandomRange(float low, float high) {
    std::mt19937 engine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> dist(low, high);
    return dist(engine);
}
