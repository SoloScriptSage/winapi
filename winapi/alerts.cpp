#include "alerts.h"

using namespace std;
using namespace chrono;

extern int currentCPUUsage;  // Assume this is updated in `cpu_monitor.cpp`
extern int currentRAMUsage;  // Assume this is updated in `ram_monitor.cpp`
extern int currentDiskUsage; // Assume this is updated in `disk_monitor.cpp`

auto lastCPUAlert = steady_clock::now();
auto lastRAMAlert = steady_clock::now();
auto lastDiskAlert = steady_clock::now();

const int ALERT_INTERVAL = 30; // Alert interval in seconds

bool ShouldShowAlert(steady_clock::time_point& lastAlertTime) {
	auto now = steady_clock::now();
	auto duration = now - lastAlertTime;  // Duration between now and last alert

	if (duration_cast<seconds>(duration).count() >= ALERT_INTERVAL) {
		lastAlertTime = now;
		return true;
	}
	return false;
}

void CheckAndShowAlerts() {
    if (currentCPUUsage > CPU_ALERT_THRESHOLD && ShouldShowAlert(lastCPUAlert)) {
        MessageBox(NULL, L"High CPU Usage! Close some apps.", L"CPU Alert", MB_OK | MB_ICONWARNING);
    }
    if (currentRAMUsage > MEMORY_ALERT_THRESHOLD && ShouldShowAlert(lastRAMAlert)) {
        MessageBox(NULL, L"High RAM Usage! Close unused programs.", L"RAM Alert", MB_OK | MB_ICONWARNING);
    }
    if (currentDiskUsage > DISK_ALERT_THRESHOLD && ShouldShowAlert(lastDiskAlert)) {
        MessageBox(NULL, L"High Disk Usage! Free up space.", L"Disk Alert", MB_OK | MB_ICONWARNING);
    }
}
