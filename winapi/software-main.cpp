#include <Windows.h> // Windows API header file
#include <psapi.h> // Process Status API header file (for process information)
#include <iphlpapi.h> // IP Helper API header file (for network information)
#include <iostream> // Include the iostream header file (for console input/output)
#include <thread> // Include the thread header file (for threading)
#include "software-definitions.h" // Include the software definitions header file

#pragma comment(lib, "iphlpapi.lib") // Link the iphlpapi library

HWND hCPU, hRAM, hNetwork; // Handles to the CPU, RAM, and Network labels

// CPU Calculation
ULONGLONG FileTimeToInt64(const FILETIME& ft) {
	return (((ULONGLONG)ft.dwHighDateTime) << 32) | ((ULONGLONG)ft.dwLowDateTime);
}

// Get the CPU usage
double GetCPUUsage() {
	static ULONGLONG prevIdle = 0, prevKernel = 0, prevUser = 0;
	FILETIME idleTime, kernelTime, userTime;

	if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
		return -1;

	ULONGLONG idle = FileTimeToInt64(idleTime);
	ULONGLONG kernel = FileTimeToInt64(kernelTime);
	ULONGLONG user = FileTimeToInt64(userTime);

	ULONGLONG idleDiff = idle - prevIdle;
	ULONGLONG kernelDiff = kernel - prevKernel;
	ULONGLONG userDiff = user - prevUser;

	prevIdle = idle;
	prevKernel = kernel;
	prevUser = user;

	if ((kernelDiff + userDiff) == 0)
		return 0;

	return 100 * (1.0 - (static_cast<double>(idleDiff) / (kernelDiff + userDiff)));
}

void GetMemoryUsage(HWND hWND) {
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (GlobalMemoryStatusEx(&statex)) {
		wchar_t buffer[256];
		swprintf(buffer, 256, L"Memory Load: %ld%%\n", statex.dwMemoryLoad);
		SetWindowText(hWND, buffer);
	}
}
// The entry point of the program
// HINSTANCE hInstance: The instance of the program
// HINSTANCE hPrevInstance: The previous instance of the program
// LPSTR lpCmdLine: The command line arguments
// int nCmdShow: The display options
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Create a new window class for the main window
	WNDCLASS SoftwareMainClass = NewWindowClass(
		(HBRUSH)COLOR_WINDOW, // Background color of the window
		LoadCursor(NULL, IDC_ARROW), // Cursor of the window
		hInstance, // Instance of the window
		LoadIcon(NULL, IDI_QUESTION), // Icon of the window
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
		L"Software Main", // Window title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window style
		CW_USEDEFAULT, // X position
		CW_USEDEFAULT, // Y position
		500, // Width
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
				case 1:
					MessageBox(hWnd, L"New menu item clicked!", L"Menu", MB_OK);
					break;
				case 2:
					MessageBox(hWnd, L"Open menu item clicked!", L"Menu", MB_OK);
					break;
				case 3:
					MessageBox(hWnd, L"Save menu item clicked!", L"Menu", MB_OK);
					break;
				case 4:
					DestroyWindow(hWnd);
					break;
				case 5:
					MessageBox(hWnd, L"Cut menu item clicked!", L"Menu", MB_OK);
					break;
				case 6:
					MessageBox(hWnd, L"Copy menu item clicked!", L"Menu", MB_OK);
					break;
				case 7:
					MessageBox(hWnd, L"Paste menu item clicked!", L"Menu", MB_OK);
					break;
			}
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_CREATE:
			MainWndAddMenus(hWnd);
			MainWndAddWidgets(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void MainWndAddMenus(HWND hWnd) {
	HMENU hMenu = CreateMenu(); // Create a new menu
	
	// Add a "File" menu
	HMENU hFileMenu = CreateMenu();
	
	HMENU hNewMenu = CreateMenu();
	AppendMenu(hNewMenu, MF_STRING, 1, L"Project");
	AppendMenu(hNewMenu, MF_STRING, 1, L"File");
	AppendMenu(hNewMenu, MF_STRING, 1, L"Repository");

	// hFileMenu is the handle to the "File" menu
	// MF_STRING is the menu item type
	// 1 is the ID of the menu item
	// L"New" is the text of the menu item
	AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hNewMenu, L"New");
	AppendMenu(hFileMenu, MF_STRING, 2, L"Open");
	AppendMenu(hFileMenu, MF_STRING, 3, L"Save");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, 4, L"Exit");
	
	// Add the "File" menu to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	
	// Add an "Edit" menu
	HMENU hEditMenu = CreateMenu();
	AppendMenu(hEditMenu, MF_STRING, 5, L"Cut");
	AppendMenu(hEditMenu, MF_STRING, 6, L"Copy");
	AppendMenu(hEditMenu, MF_STRING, 7, L"Paste");
	
	// Add the "Edit" menu to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");

	// Set the menu to the window
	SetMenu(hWnd, hMenu);
}

void MainWndAddWidgets(HWND hWnd) {
	// Create a button
	CreateWindow(
		L"BUTTON", // Button class
		L"Click Me", // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Button style
		100, // X position
		100, // Y position
		100, // Width
		50, // Height
		hWnd, // Parent window
		(HMENU)1001, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);

	// Create a textbox
	CreateWindow(
		L"EDIT", // Edit class
		L"Type here", // Default text
		WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, // Edit style
		100, // X position
		200, // Y position
		200, // Width
		100, // Height
		hWnd, // Parent window
		NULL, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);


}