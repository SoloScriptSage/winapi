#include "cpu_monitor.h"
#include "alerts.h"

using namespace std;

extern HWND hCPU; // Handle to the CPU usage label
extern atomic<bool> updateFlag;
extern auto lastCPUAlert;

// NTSTATUS: This is the return type of the function. 
// It’s a Windows-specific data type used to represent status codes for functions. 
// The function will return an NTSTATUS value, which is typically used to indicate success or error codes.

// WINAPI: This is the calling convention of the function.
// It tells the compiler how the function’s parameters should be passed and how the function should return.

// pNtQuerySystemInformation: This is a pointer to the NtQuerySystemInformation function.
// It’s a function pointer that will be used to call the NtQuerySystemInformation function dynamically.

// The code defines a function pointer type, pNtQuerySystemInformation, 
// which points to a function that can retrieve system information on Windows. 
// You would use this to call NtQuerySystemInformation, passing in the 
// right parameters to get specific system data (like process information, memory details, etc.).
// This is typically used in low - level Windows programming, often for things like system monitoring or debugging.

typedef NTSTATUS(WINAPI* pNtQuerySystemInformation)(
	ULONG SystemInformationClass, // identifier that specifies the type of system information you want to retrieve (like system performance data, process information, etc.).
	PVOID SystemInformation, // A pointer to a buffer where the requested system information will be returned.
	ULONG SystemInformationLength, // The size of the buffer in bytes.
	PULONG ReturnLength); // A pointer to a variable that will receive the size of the returned system information.

// The constant SystemProcessorPerformanceInformation corresponds to a specific type of information 
// that NtQuerySystemInformation can return. In this case, it is used 
// to request processor performance information, such as the usage statistics of the CPUs.
// 8 – SystemProcessorPerformanceInformation(Processor performance statistics, like CPU usage, etc.)

#define SystemProcessorPerformanceInformation 8

// Function to get the CPU information

vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> RetrieveCPUInfo() {
	// This line retrieves the address of the NtQuerySystemInformation function from 
	// the ntdll.dll library and assigns it to the NtQuerySystemInformation function pointer.

	// Get the address of NtQuerySystemInformation
	static pNtQuerySystemInformation NtQuerySystemInformation =
		(pNtQuerySystemInformation)

		// This retrieves the memory address of the NtQuerySystemInformation function in ntdll.dll by name.
		GetProcAddress(
			// This loads the ntdll.dll module into the process’s address space. 
			// ntdll.dll is a critical system DLL that provides many low-level functions, including NtQuerySystemInformation.
			GetModuleHandle(TEXT("ntdll.dll")),
			"NtQuerySystemInformation"
		);

	// Check if the function was loaded successfully
	if (!NtQuerySystemInformation) {
		cerr << "Failed to load NtQuerySystemInformation\n";
		return {};
	}


	ULONG numProcessors = std::thread::hardware_concurrency(); // Get the number of hardware threads
	vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> cpuInfo(numProcessors); // Create a vector of CPU information

	// This calls NtQuerySystemInformation to retrieve CPU performance information for all processors.

	// cpuInfo.data(): This passes a pointer to the data buffer (the underlying array of the cpuInfo vector) where the information will be written.

	// numProcessors * sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION): This specifies the size of the buffer in bytes. 
	// It's calculated by multiplying the number of processors by the size of each SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION structure.

	// nullptr: This indicates that we don’t need the size of the returned information.

	NTSTATUS status = NtQuerySystemInformation(SystemProcessorPerformanceInformation, cpuInfo.data(),
		numProcessors * sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION), nullptr); // Get the CPU information

	// Check if the function call was successful
	if (status != 0) { // STATUS_SUCCESS = 0
		cerr << "NtQuerySystemInformation failed\n";
		return {};
	}

	return cpuInfo;
}
// Function to print the CPU usage per core
void PrintPerCoreCPUUsage() {
	bool cpuOverloaded = false;

	auto prevCPUInfo = RetrieveCPUInfo(); // Get the previous CPU information`
	Sleep(1000); // Wait 1 second 
	auto currCPUInfo = RetrieveCPUInfo(); // Get the current CPU information

	// Check if the CPU information is empty
	if (prevCPUInfo.empty() || currCPUInfo.empty()) {
		std::cerr << "Failed to get CPU information\n";
		return;
	}


	ULONG numProcessors = thread::hardware_concurrency(); // Get the number of processors
	wstring usageText = L""; // Usage text

	// Loop through each processor
	for (ULONG i = 0; i < numProcessors; i++) {
		ULONGLONG prevIdle = prevCPUInfo[i].IdleTime.QuadPart; // Get the previous idle time
		ULONGLONG prevKernel = prevCPUInfo[i].KernelTime.QuadPart; // Get the previous kernel time
		ULONGLONG prevUser = prevCPUInfo[i].UserTime.QuadPart; // Get the previous user time

		ULONGLONG currIdle = currCPUInfo[i].IdleTime.QuadPart; // Get the current idle time
		ULONGLONG currKernel = currCPUInfo[i].KernelTime.QuadPart; // Get the current kernel time
		ULONGLONG currUser = currCPUInfo[i].UserTime.QuadPart; // Get the current user time

		// Prevent underflow
		ULONGLONG totalDiff = (currKernel > prevKernel ? (currKernel - prevKernel) : 0) +
			(currUser > prevUser ? (currUser - prevUser) : 0);
		ULONGLONG idleDiff = (currIdle > prevIdle ? (currIdle - prevIdle) : 0);

		double usage; // Calculate the CPU usage
		// Prevent division by zero
		if (totalDiff == 0) {
			usage = 0.0;
		}
		else {
			usage = (1.0 - (double)idleDiff / totalDiff) * 100.0;
		}

		// Clamp the value between 0 and 100
		usage = max(0.0, min(usage, 100.0));

		if (usage > CPU_ALERT_THRESHOLD && ShouldShowAlert(lastCPUAlert)) {
			cpuOverloaded = true;
		}
		wstringstream stream;
		stream << fixed << setprecision(2) << usage; // Set the precision of the stream

		usageText += L"CPU Core " + std::to_wstring(i) + L": " + stream.str() + L"% usage\n"; // Add the CPU usage to the text
	}

	// Update the CPU label
	if (hCPU) {
		SetWindowText(hCPU, usageText.c_str());
	}

	// Display a message box if the CPU usage is high
	/*if (cpuOverloaded) {
		MessageBox(NULL, L"High CPU Usage Detected! Close some apps.", L"CPU Alert", MB_OK | MB_ICONWARNING);
	}*/
}
// Thread function for updating CPU usage
void UpdateCPUUsage() {
	while (updateFlag) {
		PrintPerCoreCPUUsage(); // This now prints per-core CPU usage
		this_thread::sleep_for(chrono::seconds(1)); // Update every second
	}
}
