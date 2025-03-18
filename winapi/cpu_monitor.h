#pragma once
#include <windows.h>
#include <vector>
#include <atomic>
#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

extern HWND hCPU; // Handle to the CPU usage label
extern atomic<bool> updateFlag;


typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
	LARGE_INTEGER IdleTime; //  represents a 64-bit integer (used for high-precision time values). stores the amount of time the processor has spent idle (doing nothing).
	LARGE_INTEGER KernelTime; // the total time the processor has spent executing kernel-mode code (system-level operations like drivers or OS tasks).
	LARGE_INTEGER UserTime; // the total time the processor has spent executing user-mode code (applications or user tasks).
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

void PrintPerCoreCPUUsage();
void UpdateCPUUsage();
