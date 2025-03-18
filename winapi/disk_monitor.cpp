#include "disk_monitor.h"
#include "alerts.h"
#include "globals.h"
using namespace std;

atomic<bool> updateFlag;
HWND hDisk;
// Function to get the disk information
void GetDiskUsage(HWND hWND) {
	ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;

	if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalBytes, &freeBytes)) {
		double diskUsage = (1.0 - ((double)freeBytes.QuadPart / totalBytes.QuadPart)) * 100.0;

		char buffer[256];
		snprintf(buffer, 256, "Disk Usage: %.2f%%",
			(1.0 - ((double)freeBytes.QuadPart / totalBytes.QuadPart)) * 100.0);
		SetWindowTextA(hWND, buffer);

		// Check if the disk usage is above the threshold
		if (diskUsage > DISK_ALERT_THRESHOLD && ShouldShowAlert(lastRAMAlert)) {
			MessageBox(NULL, L"High Disk Usage Detected! Free up some space.", L"Disk Alert", MB_OK | MB_ICONWARNING);
		}
	}
	else {
		SetWindowTextA(hWND, "Disk Usage: Error");
	}
}
void UpdateDiskUsage() {
	while (updateFlag) {
		GetDiskUsage(hDisk);
		this_thread::sleep_for(chrono::seconds(1));
	}
}
