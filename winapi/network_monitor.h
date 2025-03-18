#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <windows.h>
#include <iphlpapi.h>
#include <thread>
#include <chrono>
#include <atomic>

#pragma comment(lib, "iphlpapi.lib")

// Function declarations
void GetNetworkUsage(HWND hWND);
void UpdateNetworkUsage();

#endif // NETWORK_MONITOR_H
