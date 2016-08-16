#include <math.h>
#include <random>
#include <thread>
#include <iostream>
#if defined(_MSC_VER)
#   include <xmmintrin.h>
#else
#   include <x86intrin.h>
#endif
// %includes%

using namespace std;

#define M128RAND _mm_set_ps(0.0f, RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f), RandomRange(100.0f, 1000.0f))
#define FLOATRAND RandomRange(100.0f, 1000.0f)

float RandomRange(float low, float high);

volatile bool TestStarted = false;
volatile bool TestFinished = false;

#pragma clang optimize off

volatile unsigned long long TestACount = 0;
void TestA() {
    // %test0.setup%
    while(TestStarted == false) { }
    while(TestFinished == false) {
        // %test0.run%
        TestACount++;
    }
}

volatile unsigned long long TestBCount = 0;
void TestB() {
    // %test1.setup%
    while(TestStarted == false) { }
    while(TestFinished == false) {
        // %test1.run%
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