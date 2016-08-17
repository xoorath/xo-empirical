#include <math.h>
#include <random>
#include <thread>
#include <iostream>
#include <fstream>
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

#define NAME_A "SSE Dot Product" // generated
#define NAME_B "Float Dot Product" // generated
#define HASH_A "b11a1168b2ce03d4790283fbd49a905c" // generated
#define HASH_B "dc8cbea71579f35b3cbf06a4f00eaf01" // generated
#define RESULT_FILE "Results/b11a1168b2ce03d4790283fbd49a905c_dc8cbea71579f35b3cbf06a4f00eaf01.txt" // generated

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
    cout << "========== Starting test.\n";
    cout << "comparing:\n\t";
    cout << NAME_A << " (" << HASH_A << ")\n\t";
    cout << NAME_B << " (" << HASH_B << ")" << endl;

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

    ofstream resultFile;
    resultFile.open(RESULT_FILE, ios::out | ios::app);
    bool failed = false;
    if(resultFile.fail()) {
        cout << "!!! writing results file failed.\n";
        failed = true;
    }
    else if(TestACount > TestBCount) {
        resultFile.precision(3);
        resultFile << NAME_A << " beat " << NAME_B << "\n";
        resultFile << "\treal:\t\t" << fixed << nanoSecondsA << "ns to " << nanoSecondsB << "ns\n";
        resultFile << "\trelative:\t" << fixed << secondsB/secondsA << "x faster\n";
    } else if(TestBCount > TestACount) {
        resultFile.precision(3);
        resultFile << NAME_B << " beat " << NAME_A << "\n";
        resultFile << "\treal:\t\t" << fixed << nanoSecondsB << "ns to " << nanoSecondsA << "ns\n";
        resultFile << "\trelative:\t" << fixed << secondsA/secondsB << "x faster\n";
    }
    else {
        cout << "there was a tie.";
        resultFile << NAME_A << " tied " << NAME_B << "\n";
        resultFile << "\treal:\t\t" << fixed << nanoSecondsA << "ns\n";
    }

    if(!failed)
        resultFile.close();

    cout << "========== Ending test." << endl;
    return 0;
}

float RandomRange(float low, float high) {
    std::mt19937 engine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> dist(low, high);
    return dist(engine);
}
