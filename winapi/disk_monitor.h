#pragma once
#include <windows.h>     // For Windows API functions (GetDiskFreeSpaceExA, SetWindowTextA, MessageBox)
#include <thread>        // For std::this_thread::sleep_for
#include <chrono>        // For chrono::seconds
#include <cstdio>        // For snprintf
#include <string>        // Optional: If you use string manipulation (though it's not directly used in the given code)
#include <atomic>

void GetDiskUsage(HWND hWND); // Function to get the disk information
void UpdateDiskUsage();      // Function to run in a separate thread for continuous disk monitoring
