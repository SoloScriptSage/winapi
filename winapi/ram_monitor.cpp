// Function to get the memory usage
void GetMemoryUsage(HWND hWND) {
	MEMORYSTATUSEX statex; // Memory status
	statex.dwLength = sizeof(statex); // Set the length of the memory status

	if (GlobalMemoryStatusEx(&statex)) { // Get the memory status
		wchar_t buffer[256]; // Buffer for the memory status
		swprintf(buffer, 256, L"Memory Load: %ld%%\n", statex.dwMemoryLoad); // Print the memory load
		SetWindowText(hWND, buffer); // Set the text of the window

		// Check if the memory load is above the threshold
		if (statex.dwMemoryLoad > MEMORY_ALERT_THRESHOLD && ShouldShowAlert(lastRAMAlert)) {
			MessageBox(NULL, L"High Memory Usage Detected! Close some apps.", L"Memory Alert", MB_OK | MB_ICONWARNING); // Display a message box
		}
	}
}// Thread function for updating memory usage
void UpdateMemoryUsage() {
	while (updateFlag) {
		GetMemoryUsage(hRAM); // Get the memory usage
		this_thread::sleep_for(chrono::seconds(1)); // Sleep for 1 second
	}
}