#include <Windows.h> // Windows API header file
#include <psapi.h> // Process Status API header file (for process information)
#include <iphlpapi.h> // IP Helper API header file (for network information)
#include <iostream> // Include the iostream header file (for console input/output)
#include <thread> // Include the thread header file (for threading)
#include "resource.h"
#include "software-definitions.h" // Include the software definitions header file
#include <string> // Include the string header file
#include <vector>
#include <iomanip> // Include for setprecision
#include <sstream> // Include for stringstream
#include <algorithm> // Required for std::clamp
#include <atomic> // Include the atomic header file

#pragma comment(lib, "iphlpapi.lib") // Link the iphlpapi library

#define WM_START_THREADS (WM_USER + 1)

using namespace std;

HWND hCPU, hRAM, hNetwork, hDisk; // Handles to the CPU, RAM, and Network labels
atomic<bool> updateFlag = true; // Flag to control the update loop

thread cpuThread; // Thread for updating CPU usage
thread ramThread; // Thread for updating RAM usage
thread diskThread; // Thread for updating disk usage
thread networkThread; // Thread for updating network usage

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
	LARGE_INTEGER IdleTime; //  represents a 64-bit integer (used for high-precision time values). stores the amount of time the processor has spent idle (doing nothing).
	LARGE_INTEGER KernelTime; // the total time the processor has spent executing kernel-mode code (system-level operations like drivers or OS tasks).
	LARGE_INTEGER UserTime; // the total time the processor has spent executing user-mode code (applications or user tasks).
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

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
vector<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION> GetCPUInfo() {
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
	auto prevCPUInfo = GetCPUInfo(); // Get the previous CPU information
	Sleep(1000); // Wait 1 second 
	auto currCPUInfo = GetCPUInfo(); // Get the current CPU information

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

		wstringstream stream;
		stream << fixed << setprecision(2) << usage; // Set the precision of the stream

		usageText += L"CPU Core " + std::to_wstring(i) + L": " + stream.str() + L"% usage\n"; // Add the CPU usage to the text
	}

	// Update the CPU label
	if (hCPU) {
		SetWindowText(hCPU, usageText.c_str());
	}
}
// Thread function for updating CPU usage
void UpdateCPUUsage() {
	while (updateFlag) {
		PrintPerCoreCPUUsage(); // This now prints per-core CPU usage
		this_thread::sleep_for(chrono::seconds(1)); // Update every second
	}
}

// Function to get the memory usage
void GetMemoryUsage(HWND hWND) {
	MEMORYSTATUSEX statex; // Memory status
	statex.dwLength = sizeof(statex); // Set the length of the memory status

	if (GlobalMemoryStatusEx(&statex)) { // Get the memory status
		wchar_t buffer[256]; // Buffer for the memory status
		swprintf(buffer, 256, L"Memory Load: %ld%%\n", statex.dwMemoryLoad); // Print the memory load
		SetWindowText(hWND, buffer); // Set the text of the window
	}
}// Thread function for updating memory usage
void UpdateMemoryUsage() {
	while (updateFlag) {
		GetMemoryUsage(hRAM); // Get the memory usage
		this_thread::sleep_for(chrono::seconds(1)); // Sleep for 1 second
	}
}

// Function to get the disk information
void GetDiskUsage(HWND hWND) {
	ULARGE_INTEGER freeBytesAvailable, totalBytes, freeBytes;

	if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalBytes, &freeBytes)) {
		char buffer[256];
		snprintf(buffer, 256, "Disk Usage: %.2f%%",
			(1.0 - ((double)freeBytes.QuadPart / totalBytes.QuadPart)) * 100.0);
		SetWindowTextA(hWND, buffer);
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

// The entry point of the program
// HINSTANCE hInstance: The instance of the program
// HINSTANCE hPrevInstance: The previous instance of the program
// LPSTR lpCmdLine: The command line arguments
// int nCmdShow: The display options
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	fontRectangle = CreateFontA(
		20, // Height of the font
		0, // Width of the font
		0, // Angle of escapement
		0, // Orientation angle
		FW_NORMAL, // Font weight
		FALSE, // Italic
		FALSE, // Underline
		FALSE, // Strikeout
		ANSI_CHARSET, // Character set
		OUT_DEFAULT_PRECIS, // Output precision
		CLIP_DEFAULT_PRECIS, // Clipping precision
		DEFAULT_QUALITY, // Output quality
		DEFAULT_PITCH, // Pitch and family
		"Consolas" // Font name
	);

	// Create a new window class for the main window
	WNDCLASS SoftwareMainClass = NewWindowClass(
		(HBRUSH)COLOR_WINDOW, // Background color of the window
		LoadCursor(NULL, IDC_ARROW), // Cursor of the window
		hInstance, // Instance of the window
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)), // Icon of the window
		L"MainWNDClass", // Name of the window class
		SoftwareMainProcedure // Window procedure of the window
	);

	// Register the window class
	// If registration fails, show an error message and return -1
	if (!RegisterClass(&SoftwareMainClass)) {
		MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	
	MSG SoftwareMainMessage = { 0 }; // Message structure for the main window

	// Create the main window
	CreateWindow(
		L"MainWNDClass", // Window class name
		L"WinApi", // Window title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window style
		CW_USEDEFAULT, // X position
		CW_USEDEFAULT, // Y position
		700, // Width
		500, // Height
		NULL, // Parent window
		NULL, // Menu
		hInstance, // Instance
		NULL // Additional data
	);

	// Message loop for the main window
	while (GetMessage(&SoftwareMainMessage, NULL, 0, 0)) {
		TranslateMessage(&SoftwareMainMessage); // Translate the message
		DispatchMessage(&SoftwareMainMessage); // Dispatch the message
	}

	return 0;
}

// This is a function definition for creating a new window class, which defines the properties and behavior of a window
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS NWC = { 0 }; // New window class

	NWC.hIcon = Icon; // Icon of the window
	NWC.hCursor = Cursor; // Cursor of the window
	NWC.hInstance = hInst; // Instance of the window
	NWC.lpszClassName = Name; // Name of the window class
	NWC.hbrBackground = BGColor; // Background color of the window
	NWC.lpfnWndProc = Procedure; // Window procedure of the window

	return NWC;
}

// This is a function definition for the window procedure. 
// The function is used to process messages sent to the window (like button clicks, key presses, etc.)
// HWND hWnd: The handle to the window
// UINT uMsg: The message sent to the window
// WPARAM wParam: Additional message information
// LPARAM lParam: Additional message information
LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	// Process the message
	switch (uMsg) {
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case MENU_NEW:
					MessageBox(hWnd, L"New menu item clicked!", L"Menu", MB_OK);
					break;
				case MENU_OPEN:
					/*LoadData("D:\\Projects\\winapi\\output.txt");*/
					if (GetOpenFileNameA(&ofn)) {
						LoadData(ofn.lpstrFile);
					}break;
				case MENU_SAVE:
					if (GetSaveFileNameA(&ofn)) {
						SaveData(ofn.lpstrFile);
					}
					/*SaveData("D:\\Projects\\winapi\\output.txt");*/
					break;
				case MENU_EXIT:
					DestroyWindow(hWnd);
					break;
				case MENU_CUT:
					MessageBox(hWnd, L"Cut menu item clicked!", L"Menu", MB_OK);
					break;
				case MENU_COPY:
					MessageBox(hWnd, L"Copy menu item clicked!", L"Menu", MB_OK);
					break;
				case MENU_PASTE:
					MessageBox(hWnd, L"Paste menu item clicked!", L"Menu", MB_OK);
					break;
				case BTN_READ:
					// Read Strings with Buffer
					// readChars = GetWindowTextA(hEditControl, Buffer, TEXT_BUFFER_SIZE); // Get the text from the edit control
					// SetWindowTextA(hStaticControl, Buffer); // Set the text of the static control
					// SetWindowTextA(hStaticControl, ("Symbols read: " + to_string(readChars)).c_str()); // Set the text of the static control
					num = GetDlgItemInt(hWnd, DIGIT_INDEX_NUMBER, NULL, FALSE);
					SetWindowTextA(hStaticControl, ("Number read: " + to_string(num)).c_str());

					break;
				case BTN_READ_CLR:
					clrR = GetDlgItemInt(hWnd, DLG_INDEX_COLOR_R, NULL, FALSE);
					clrG = GetDlgItemInt(hWnd, DLG_INDEX_COLOR_G, NULL, FALSE);
					clrB = GetDlgItemInt(hWnd, DLG_INDEX_COLOR_B, NULL, FALSE);

					brushRectangle = CreateSolidBrush(RGB(clrR, clrG, clrB));
					fontColor = RGB(255 - clrR, 255 - clrG, 255 - clrB);

					RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
					break;
				case BTN_CLS:
					SetWindowText(hEditControl, L"");
					break;
			}break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_CREATE:
			MainWndAddMenus(hWnd);
			MainWndAddWidgets(hWnd);
			SetOpenFileParameters(hWnd);

			SendMessage(hStaticControl, WM_SETFONT, (WPARAM)fontRectangle, TRUE);
			// Post a message to start the threads after the widgets are added
			PostMessage(hWnd, WM_START_THREADS, 0, 0);
			break;

		case WM_START_THREADS:
			// Start the threads after widgets are added
			cpuThread = thread(UpdateCPUUsage); // Create and start the thread for CPU usage
			ramThread = thread(UpdateMemoryUsage); // Create and start the thread for memory usage
			diskThread = thread(UpdateDiskUsage); // Create and start the thread for disk usage
			networkThread = thread(UpdateNetworkUsage); // Start network monitoring

			break;
		case WM_DESTROY:
			updateFlag = false;

			if (cpuThread.joinable()) cpuThread.join();
			if (ramThread.joinable()) ramThread.join();			
			if (diskThread.joinable()) ramThread.join();
			if (networkThread.joinable()) networkThread.join();

			DeleteObject(brushRectangle);
			DeleteObject(fontRectangle);

			PostQuitMessage(0);
			break;
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			FillRect(ps.hdc, &rc, brushRectangle);

			SetBkMode(ps.hdc, TRANSPARENT);
			SetTextColor(ps.hdc, fontColor);
			SelectObject(ps.hdc, fontRectangle);
			DrawTextA(ps.hdc, "Hello, WinAPI!", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			EndPaint(hWnd, &ps);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

// Widgets and Menus

void MainWndAddMenus(HWND hWnd) {
	HMENU hMenu = CreateMenu(); // Create a general menu
	
	HMENU hFileMenu = CreateMenu(); // Create a "File" menu
	HMENU hNewMenu = CreateMenu(); // Create a "New" menu
	HMENU hEditMenu = CreateMenu(); // Create an "Edit" menu

	// hFileMenu is the handle to the "File" menu
	// MF_STRING is the menu item type
	// 1 is the ID of the menu item
	// L"New" is the text of the menu item
	AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hNewMenu, L"New");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, 2002, L"Open");
	AppendMenu(hFileMenu, MF_STRING, 2003, L"Save");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, 2004, L"Exit");
	// Add the "File" menu to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	
	// Adding elements to the "New" menu
	AppendMenu(hNewMenu, MF_STRING, NULL, L"Project");
	AppendMenu(hNewMenu, MF_STRING, NULL, L"File");
	AppendMenu(hNewMenu, MF_STRING, NULL, L"Repository");

	// Adding elements to the "Edit" menu
	AppendMenu(hEditMenu, MF_STRING, 2005, L"Cut");
	AppendMenu(hEditMenu, MF_STRING, 2006, L"Copy");
	AppendMenu(hEditMenu, MF_STRING, 2007, L"Paste");

	// Add the "Edit" menu to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");

	// Set the menu to the window
	SetMenu(hWnd, hMenu);
}

void MainWndAddWidgets(HWND hWnd) {

	// Create label: "Enter Text Below"
	hStaticControl = CreateWindow(
		L"STATIC",              // Static class
		L"Enter Text Below:",   // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		10,                // X position - 10
		10,                // Y position (top-left) - 10
		300,          // Width - 300 
		25,           // Height - 25
		hWnd,                  // Parent window
		NULL,                  // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create multiline text box for user input
	hEditControl = CreateWindow(
		L"EDIT",                      // Edit class
		L"Default Text",               // Default text
		WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, // Edit style
		10,                       // X position - 10
		35,          // Y position (below the label) - 35
		300,                 // Width - 300
		200,                // Height - 200
		hWnd,                         // Parent window
		NULL,                         // Menu ID
		NULL,                         // Instance
		NULL                          // Additional data
	);

	// Create number input box (centered text, for numeric input)
	hNumberControl = CreateWindow(
		L"EDIT",               // Edit class
		L"0",                  // Default text
		WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, // Edit style
		10,               // X position - 10
		240, // Y position (below the text box) - 235
		300,         // Width - 300
		30,        // Height - 30
		hWnd,                 // Parent window
		(HMENU)DIGIT_INDEX_NUMBER, // Menu ID
		NULL,                 // Instance
		NULL                  // Additional data
	);

	// Create "Clear" button (to the right of the textbox)
	CreateWindow(
		L"BUTTON",              // Button class
		L"Clear",               // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Button style
		10, // X position (right of the textbox)
		275,               // Y position
		145,          // Width
		30,         // Height
		hWnd,                  // Parent window
		(HMENU)1003,           // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create "Read" button (next to "Clear")
	CreateWindow(
		L"BUTTON",              // Button class
		L"Read",                // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Button style
		165, // X position (next to "Clear" button)
		275, // Y position (below "Clear" button)
		145,          // Width
		30,         // Height
		hWnd,                  // Parent window
		(HMENU)1004,           // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create label for R value
	CreateWindow(
		L"STATIC",             // Static class
		L"R:",     // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		10,                // X position - 10
		310, // Y position (below the "Read" button) - 275
		30,          // Width - 5
		30,         // Height - 5
		hWnd,                  // Parent window
		NULL,                  // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create a textbox to the right of the label R
	CreateWindow(
		L"EDIT",               // Edit class
		L"0",                  // Default text
		WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, // Edit style
		25,               // X position - 40
		310, // Y position (below the "Read" button) - 275
		30,          // Width - 30
		25,         // Height - 30
		hWnd,                  // Parent window
		(HMENU)DLG_INDEX_COLOR_R, // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create label for G value
	CreateWindow(
		L"STATIC",             // Static class
		L"G:",     // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		60,                // X position - 10
		310, // Y position (below the "Read" button) - 275
		30,          // Width - 5
		30,         // Height - 5
		hWnd,                  // Parent window
		NULL,                  // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);
	
	// Create a textbox to the right of the label G
	CreateWindow(
		L"EDIT",               // Edit class
		L"0",                  // Default text
		WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, // Edit style
		75,               // X position - 40
		310, // Y position (below the "Read" button) - 275
		30,          // Width - 30
		25,         // Height - 30
		hWnd,                  // Parent window
		(HMENU)DLG_INDEX_COLOR_G, // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create label for B value
	CreateWindow(
		L"STATIC",             // Static class
		L"B:",     // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		110,                // X position - 10
		310, // Y position (below the "Read" button) - 275
		30,          // Width - 5
		30,         // Height - 5
		hWnd,                  // Parent window
		NULL,                  // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create a textbox to the right of the label B
	CreateWindow(
		L"EDIT",               // Edit class
		L"0",                  // Default text
		WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, // Edit style
		125,               // X position - 40
		310, // Y position (below the "Read" button) - 275
		30,          // Width - 30
		25,         // Height - 30
		hWnd,                  // Parent window
		(HMENU)DLG_INDEX_COLOR_B, // Menu ID
		NULL,                  // Instance
		NULL                   // Additional data
	);

	// Create a button on the bottom readcolor
	CreateWindow(
		L"BUTTON",              // Button class
		L"Read Color",          // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Button style
		10, // X position (right of the textbox)
		340, // Y position (below the buttons)
		300,         // Width
		30,          // Height
		hWnd,                 // Parent window
		(HMENU)BTN_READ_CLR,                 // Menu ID
		NULL,                 // Instance
		NULL                  // Additional data
	);

	// x1 - 10 // y1 - 350
	// x2 - 310 // y2 - 375
	rc = {310, 380, 10, 415}; // x2, y1, x1, y2

	// Create static label for CPU usage, positioned to the right of the textbox
	hCPU = CreateWindow(
		L"STATIC",             // Static class
		L"CPU Usage: 0%",      // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		325, // X position (right of the textbox)
		35, // Y position (below the buttons)
		300,         // Width
		300,          // Height
		hWnd,                 // Parent window
		NULL,                 // Menu ID
		NULL,                 // Instance
		NULL                  // Additional data
	);

	// Create static label for Memory load, positioned below the CPU usage label
	hRAM = CreateWindow(
		L"STATIC",             // Static class
		L"Memory Load: 0%",    // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		325, // X position (right of the textbox)
		335, // Y position (below CPU label)
		300,         // Width
		20,          // Height
		hWnd,                 // Parent window
		NULL,                 // Menu ID
		NULL,                 // Instance
		NULL                  // Additional data
	);

	hDisk = CreateWindow(
		L"STATIC",             // Static class
		L"Disk Usage: 0%",    // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		325, // X position (right of the textbox)
		355, // Y position (below CPU label)
		300,         // Width
		20,          // Height
		hWnd,                 // Parent window
		NULL,                 // Menu ID
		NULL,                 // Instance
		NULL                  // Additional data
	);
	
	hNetwork = CreateWindow(
		L"STATIC",             // Static class
		L"Network Usage: 0 bytes sent, 0 bytes received",    // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		325, // X position (right of the textbox)
		375, // Y position (below CPU label)
		300,         // Width
		20,          // Height
		hWnd,                 // Parent window
		NULL,                 // Menu ID
		NULL,                 // Instance
		NULL                  // Additional data
	);
}

// File Processing

void SaveData(LPCSTR path) {
	HANDLE FileToSave = CreateFileA(
		path, 
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL
	); // Create a new file

	int saveLength = GetWindowTextLength(hEditControl); // Get the length of the text in the edit control
	char* saveBuffer = new char[saveLength + 1]; // Create a buffer to store the text

	saveLength = GetWindowTextA(hEditControl, saveBuffer, saveLength + 1); // Get the text from the edit control

	DWORD bytesIterated; // Bytes written to the file
	WriteFile(FileToSave, saveBuffer, saveLength, &bytesIterated, NULL); // Write the text to the file

	// **Explicitly truncate the file after writing**
	SetFilePointer(FileToSave, bytesIterated, NULL, FILE_BEGIN); // Move pointer to the correct position
	SetEndOfFile(FileToSave); // Truncate anything beyond the written data

	CloseHandle(FileToSave); // Close the file
	delete[] saveBuffer; // Delete the buffer
}

void LoadData(LPCSTR path) {
	HANDLE FileToLoad = CreateFileA(
		path,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	); // Open the file

	DWORD bytesIterated;
	ReadFile(
		FileToLoad, 
		Buffer, 
		TEXT_BUFFER_SIZE, 
		&bytesIterated, 
		NULL); // Read the file

	SetWindowTextA(hEditControl, Buffer); // Set the text of the edit control
	CloseHandle(FileToLoad); // Close the file
}

void SetOpenFileParameters(HWND hWND) {
	ZeroMemory(&ofn, sizeof(ofn)); // Clear the memory of the open file dialog

	ofn.lStructSize = sizeof(ofn); // Set the size of the structure
	ofn.hwndOwner = hWND; // Set the owner of the dialog
	ofn.lpstrFile = filename; // Set the file name
	ofn.nMaxFile = sizeof(filename); // Set the maximum file size
	ofn.lpstrFilter = "*.txt";
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = "D:\\Projects\\winapi";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}