#pragma once

#include <Windows.h>
#include <chrono>
#include <vector>
#include <atomic>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

extern atomic<bool>updateFlag;
extern HWND hCPU, hRAM, hNetwork, hDisk; // Handles to the CPU, RAM, and Network labels;

extern chrono::steady_clock::time_point lastCPUAlert;
extern chrono::steady_clock::time_point lastRAMAlert;
extern chrono::steady_clock::time_point lastDiskAlert;
