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
// %includes%

#define NAME_A "" // %test0.name%
#define NAME_B "" // %test1.name%

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