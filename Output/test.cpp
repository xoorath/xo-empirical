#include <math.h>
#include <random>
#include <thread>
#include <iostream>
#if defined(_MSC_VER)
#   include <xmmintrin.h>
#else
#   include <x86intrin.h>
#endif
#if defined(__clang__) && defined(_WIN32)
#   include <windows.h>
#   define SleepSeconds(x) Sleep(x*1000)
#else
#   define SleepSeconds(x) this_thread::sleep_for(chrono::seconds(x))
#endif

#define NAME_A "SSE 4.2 Dot Product" // generated
#define NAME_B "Float Dot Product" // generated

using namespace std;

#define M128RAND _mm_set_ps(0.0f, RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f))
#define FLOATRAND RandomRange(100.0f, 1000.0f)

float RandomRange(float low, float high);

volatile bool TestStarted = false;
volatile bool TestFinished = false;

#pragma clang optimize off

volatile unsigned long long TestACount = 0;
void TestA() {
    // Test 0: SSE 4.2 Dot Product
    // Domain: xo-math
    // Sub-Domain: Dot Product
    // Technology: SSE 4.2

    ////////// Setup (generated):
    __m128 a = M128RAND;
    __m128 b = M128RAND;
    float dot;
    ////////// End Setup (generated):
    while(TestStarted == false) { }
    while(TestFinished == false) {
        ////////// Run (generated):
        dot = _mm_cvtss_f32(_mm_dp_ps(a, b, 0x7f));
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
    cout << "========== Starting test.\n";
    cout << "comparing:\n\t";
    cout << NAME_A << "\n\t";
    cout << NAME_B << endl;

    std::thread threadA(&TestA);
    std::thread threadB(&TestB);

    int seconds = 10;
    TestStarted = true;

    for(int i = 0; i < seconds; ++i) {
        cout << "Running test. " << (seconds - i) << " seconds remain." << endl;
        SleepSeconds(1);
    }

    TestFinished = true;

    threadA.join();
    threadB.join();

    double secondsA = (double)seconds / (double)TestACount;
    double nanoSecondsA = secondsA * 1.0e+9;
    double secondsB = (double)seconds / (double)TestBCount;
    double nanoSecondsB = secondsB * 1.0e+9;
    cout.precision(3);
    cout << NAME_A << ":\n\t" << fixed << nanoSecondsA << " ns" << endl;
    cout << NAME_B << ":\n\t" << fixed << nanoSecondsB << " ns" << endl;

    cout << "========== Ending test." << endl;
    return 0;
}

float RandomRange(float low, float high) {
    std::mt19937 engine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> dist(low, high);
    return dist(engine);
}
