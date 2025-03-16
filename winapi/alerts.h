#ifndef ALERTS_H
#define ALERTS_H

#include <Windows.h>
#include <chrono>

bool ShouldShowAlert(std::chrono::steady_clock::time_point& lastAlertTime);
void CheckAndShowAlerts();

#endif // ALERTS_H