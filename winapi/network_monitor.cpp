#include "network_monitor.h"
#include "globals.h"

atomic<bool> updateFlag;

// Function to get the network information
void GetNetworkUsage(HWND hWND) {
	MIB_IFTABLE* pIfTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	if (GetIfTable(NULL, &dwSize, false) == ERROR_INSUFFICIENT_BUFFER) {
		pIfTable = (MIB_IFTABLE*)malloc(dwSize);
		if (pIfTable == NULL) {
			SetWindowText(hWND, L"Network Usage: Memory Error");
			return;
		}
	}
	else {
		SetWindowText(hWND, L"Network Usage: Failed");
		return;
	}

	if ((dwRetVal = GetIfTable(pIfTable, &dwSize, false)) == NO_ERROR) {
		ULONGLONG totalSent = 0, totalReceived = 0;

		for (DWORD i = 0; i < pIfTable->dwNumEntries; i++) {
			// Prefer ullOutOctets if available (64-bit)
			ULONGLONG sentBytes = (pIfTable->table[i].dwOutOctets);
			ULONGLONG receivedBytes = (pIfTable->table[i].dwInOctets);

			if (pIfTable->table[i].dwType == IF_TYPE_ETHERNET_CSMACD ||
				pIfTable->table[i].dwType == IF_TYPE_IEEE80211) { // Ethernet or WiFi
				totalSent += sentBytes;
				totalReceived += receivedBytes;
			}
		}

		static ULONGLONG prevSent = 0, prevReceived = 0;

		// Handle counter resets (e.g., if network interface is reconnected)
		if (prevSent > totalSent) prevSent = totalSent;
		if (prevReceived > totalReceived) prevReceived = totalReceived;

		ULONGLONG sentSpeed = totalSent - prevSent;
		ULONGLONG receivedSpeed = totalReceived - prevReceived;

		prevSent = totalSent;
		prevReceived = totalReceived;

		wchar_t buffer[256];
		swprintf(buffer, 256, L"Upload: %.2f KB/s | Download: %.2f KB/s",
			sentSpeed / 1024.0, receivedSpeed / 1024.0);
		SetWindowText(hWND, buffer);
	}
	else {
		SetWindowText(hWND, L"Network Usage: Error");
	}

	free(pIfTable);
}

void UpdateNetworkUsage() {
	while (updateFlag) {
		GetNetworkUsage(hNetwork);
		this_thread::sleep_for(chrono::seconds(1)); // Update every second
	}
}
