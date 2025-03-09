#include <Windows.h> // Windows API header file
#include <psapi.h> // Process Status API header file (for process information)
#include <iphlpapi.h> // IP Helper API header file (for network information)
#include <iostream> // Include the iostream header file (for console input/output)
#include <thread> // Include the thread header file (for threading)
#include "resource.h"
#include "software-definitions.h" // Include the software definitions header file
#include <string> // Include the string header file
#pragma comment(lib, "iphlpapi.lib") // Link the iphlpapi library

HWND hCPU, hRAM, hNetwork; // Handles to the CPU, RAM, and Network labels

using namespace std;

// CPU Calculation
ULONGLONG FileTimeToInt64(const FILETIME& ft) {
	return (((ULONGLONG)ft.dwHighDateTime) << 32) | ((ULONGLONG)ft.dwLowDateTime);
}

// Get the CPU usage
double GetCPUUsage() {
	static ULONGLONG prevIdle = 0, prevKernel = 0, prevUser = 0; // Previous idle, kernel, and user times
	FILETIME idleTime, kernelTime, userTime; // Idle, kernel, and user times

	if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) // Get the system times
		return -1;

	ULONGLONG idle = FileTimeToInt64(idleTime);  // Idle time
	ULONGLONG kernel = FileTimeToInt64(kernelTime); // Kernel time
	ULONGLONG user = FileTimeToInt64(userTime);		// User time

	ULONGLONG idleDiff = idle - prevIdle; // Idle difference
	ULONGLONG kernelDiff = kernel - prevKernel; // Kernel difference
	ULONGLONG userDiff = user - prevUser; // User difference 

	prevIdle = idle; // Set the previous idle time
	prevKernel = kernel; // Set the previous kernel time
	prevUser = user; // Set the previous user time

	if ((kernelDiff + userDiff) == 0) // If the kernel and user difference is 0, return 0
		return 0;

	return 100 * (1.0 - (static_cast<double>(idleDiff) / (kernelDiff + userDiff))); // Return the CPU usage
}

void GetMemoryUsage(HWND hWND) {
	MEMORYSTATUSEX statex; // Memory status
	statex.dwLength = sizeof(statex); // Set the length of the memory status

	if (GlobalMemoryStatusEx(&statex)) { // Get the memory status
		wchar_t buffer[256]; // Buffer for the memory status
		swprintf(buffer, 256, L"Memory Load: %ld%%\n", statex.dwMemoryLoad); // Print the memory load
		SetWindowText(hWND, buffer); // Set the text of the window
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
				case MENU_NEW:
					MessageBox(hWnd, L"New menu item clicked!", L"Menu", MB_OK);
					break;
				case MENU_OPEN:
					MessageBox(hWnd, L"Open menu item clicked!", L"Menu", MB_OK);
					break;
				case MENU_SAVE:
					MessageBox(hWnd, L"Save menu item clicked!", L"Menu", MB_OK);
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
	
	HMENU hFileMenu = CreateMenu(); // Add a "File" menu
	HMENU hNewMenu = CreateMenu();
	
	// hFileMenu is the handle to the "File" menu
	// MF_STRING is the menu item type
	// 1 is the ID of the menu item
	// L"New" is the text of the menu item
	AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hNewMenu, L"New");

	AppendMenu(hFileMenu, MF_STRING, 2002, L"Open");
	AppendMenu(hFileMenu, MF_STRING, 2003, L"Save");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, 2004, L"Exit");

	// Add the "File" menu to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

	AppendMenu(hNewMenu, MF_STRING, NULL, L"Project");
	AppendMenu(hNewMenu, MF_STRING, NULL, L"File");
	AppendMenu(hNewMenu, MF_STRING, NULL, L"Repository");

	// Add an "Edit" menu
	HMENU hEditMenu = CreateMenu();
	AppendMenu(hEditMenu, MF_STRING, 2005, L"Cut");
	AppendMenu(hEditMenu, MF_STRING, 2006, L"Copy");
	AppendMenu(hEditMenu, MF_STRING, 2007, L"Paste");

	// Add the "Edit" menu to the main menu
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");

	// Set the menu to the window
	SetMenu(hWnd, hMenu);
}

void MainWndAddWidgets(HWND hWnd) {
	int windowWidth = 500;
	int windowHeight = 600; // Adjusted for better spacing

	int menuHeight = 30; // Approximate height of the menu
	int labelHeight = 25;
	int labelY = menuHeight + 10; // Position label below menu with some padding

	int textBoxHeight = 200; // Adjusted height
	int elementWidth = 300;
	int elementHeight = 30;

	int centerX = (windowWidth - elementWidth) / 2;

	// Create a label below the menu
	hStaticControl = CreateWindow(
		L"STATIC", // Static class
		L"Enter Text Below:", // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		centerX, // X position
		labelY, // Y position (below the menu)
		elementWidth, // Width
		labelHeight, // Height
		hWnd, // Parent window
		NULL, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);

	// Create a textbox below the label
	int textBoxY = labelY + labelHeight + 10; // Positioning textbox below the label
	hEditControl = CreateWindow(
		L"EDIT", // Edit class
		L"Default Text", // Default text
		WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, // Edit style
		centerX, // X position
		textBoxY, // Y position
		elementWidth, // Width
		textBoxHeight, // Height
		hWnd, // Parent window
		NULL, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);

	// Create a number input box below the text box
	int numberBoxY = textBoxY + textBoxHeight + 10;
	hNumberControl = CreateWindow(
		L"EDIT", // Edit class
		L"0", // Default text
		WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, // Edit style
		centerX, // X position
		numberBoxY, // Y position
		elementWidth, // Width
		elementHeight, // Height
		hWnd, // Parent window
		(HMENU)DIGIT_INDEX_NUMBER, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);

	// Create a button below the number input box
	int buttonY = numberBoxY + elementHeight + 10; // **Fix: Positioning below number input**
	CreateWindow(
		L"BUTTON", // Button class
		L"Clear", // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Button style
		centerX, // X position
		buttonY, // Y position
		elementWidth, // Width
		elementHeight, // Height
		hWnd, // Parent window
		(HMENU)1003, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);

	// Create Read button below Clear button
	CreateWindow(
		L"BUTTON", // Button class
		L"Read", // Button text
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, // Button style
		centerX, // X position
		buttonY + elementHeight + 10, // **Fix: Positioning below Clear button**
		elementWidth, // Width
		elementHeight, // Height
		hWnd, // Parent window
		(HMENU)1004, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);
}


