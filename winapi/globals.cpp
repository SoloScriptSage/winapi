#include "globals.h"

using namespace std;
using namespace chrono;

atomic<bool> updateFlag = true; // Flag to control the update loop
HWND hCPU, hRAM, hNetwork, hDisk;

steady_clock::time_point lastCPUAlert = steady_clock::now();
steady_clock::time_point lastRAMAlert = steady_clock::now();
steady_clock::time_point lastDiskAlert = steady_clock::now();
