#pragma once

#include <windows.h>
#include "alerts.h"
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

// Function declarations
void GetMemoryUsage(HWND hWND); // Function to get the current memory usage
void UpdateMemoryUsage(); // Function to run in a separate thread for continuous memory monitoring
bool ShouldShowAlert(std::chrono::steady_clock::time_point& lastAlertTime); // Check if enough time has passed since the last alert
