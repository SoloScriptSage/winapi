#ifndef ALERTS_H
#define ALERTS_H

#include <Windows.h>
#include <chrono>

#define CPU_ALERT_THRESHOLD 80 // CPU usage alert threshold
#define MEMORY_ALERT_THRESHOLD 80 // Memory usage alert threshold
#define DISK_ALERT_THRESHOLD 80 // Disk usage alert threshold

bool ShouldShowAlert(std::chrono::steady_clock::time_point& lastAlertTime);
void CheckAndShowAlerts();
void AlertThread();

#endif // ALERTS_H
