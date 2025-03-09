#include <Windows.h> // Windows API header file
#include <psapi.h> // Process Status API header file (for process information)
#include <iphlpapi.h> // IP Helper API header file (for network information)
#include <iostream> // Include the iostream header file (for console input/output)
#include <thread> // Include the thread header file (for threading)
#include "resource.h"
#include "software-definitions.h" // Include the software definitions header file
#include <string> // Include the string header file
#pragma comment(lib, "iphlpapi.lib") // Link the iphlpapi library

#define WM_START_THREADS (WM_USER + 1)

using namespace std;

HWND hCPU, hRAM, hNetwork; // Handles to the CPU, RAM, and Network labels
atomic<bool> updateFlag = true; // Flag to control the update loop

thread cpuThread;
thread ramThread;

// CPU Calculation
ULONGLONG FileTimeToInt64(const FILETIME& ft) {
	return (((ULONGLONG)ft.dwHighDateTime) << 32) | ((ULONGLONG)ft.dwLowDateTime);
}

// Get the CPU usage
double GetCPUUsage() {
	static ULONGLONG prevIdleTime = 0, prevKernelTime = 0, prevUserTime = 0;
	FILETIME idleTime, kernelTime, userTime;

	if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
		return -1; // Failed to retrieve times
	}

	ULONGLONG idle = FileTimeToInt64(idleTime);
	ULONGLONG kernel = FileTimeToInt64(kernelTime);
	ULONGLONG user = FileTimeToInt64(userTime);

	if (prevIdleTime == 0) { // First call, store initial values
		prevIdleTime = idle;
		prevKernelTime = kernel;
		prevUserTime = user;
		return 0.0;
	}

	ULONGLONG idleDiff = idle - prevIdleTime;
	ULONGLONG kernelDiff = kernel - prevKernelTime;
	ULONGLONG userDiff = user - prevUserTime;

	prevIdleTime = idle;
	prevKernelTime = kernel;
	prevUserTime = user;

	ULONGLONG totalTime = kernelDiff + userDiff;
	if (totalTime == 0) return 0.0;

	return 100.0 * (1.0 - (static_cast<double>(idleDiff) / totalTime));
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

// Thread function for updating CPU usage
void UpdateCPUUsage() {
	while (updateFlag) {
		double cpuUsage = GetCPUUsage();
		if (hCPU) {
			wchar_t buffer[256];
			swprintf(buffer, 256, L"CPU Usage: %.2f%%", cpuUsage);
			PostMessage(hCPU, WM_SETTEXT, 0, (LPARAM)buffer);
		}
		this_thread::sleep_for(chrono::seconds(1)); // Update every second
	}
}

// Thread function for updating memory usage
void UpdateMemoryUsage() {
	while (updateFlag) {
		GetMemoryUsage(hRAM); // Get the memory usage
		this_thread::sleep_for(chrono::seconds(1)); // Sleep for 1 second
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

			// Post a message to start the threads after the widgets are added
			PostMessage(hWnd, WM_START_THREADS, 0, 0);
			break;
		case WM_START_THREADS:
			// Start the threads after widgets are added
			cpuThread = thread(UpdateCPUUsage); // Create and start the thread for CPU usage
			ramThread = thread(UpdateMemoryUsage); // Create and start the thread for memory usage
			break;
		case WM_DESTROY:
			if (cpuThread.joinable())
				cpuThread.join();
			if (ramThread.joinable())
				ramThread.join();

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
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
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

	hCPU = CreateWindow(
		L"STATIC", // Static class
		L"CPU Usage: 0%", // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		centerX, // X position
		buttonY + elementHeight + 50, // Y position
		elementWidth, // Width
		labelHeight, // Height
		hWnd, // Parent window
		NULL, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);

	hRAM = CreateWindow(
		L"STATIC", // Static class
		L"Memory Load: 0%", // Label text
		WS_VISIBLE | WS_CHILD | SS_LEFT, // Label style
		centerX, // X position
		buttonY + elementHeight + 80, // Y position
		elementWidth, // Width
		labelHeight, // Height
		hWnd, // Parent window
		NULL, // Menu ID
		NULL, // Instance
		NULL // Additional data
	);
}

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

void UpdateStats() {
	while (true) {
		double cpuUsage = GetCPUUsage(); // Get the CPU usage

		MEMORYSTATUS statex; // Memory status
		statex.dwLength = sizeof(statex); // Set the length of the memory status
		
		wchar_t cpuBuffer[256]; // Buffer for the CPU usage
		wchar_t ramBuffer[256]; // Buffer for the RAM usage

		swprintf(cpuBuffer, 256, L"CPU Usage: %.2f%%\n", cpuUsage); // Print the CPU usage
		swprintf(ramBuffer, 256, L"Memory Load: %ld%%\n", statex.dwMemoryLoad); // Print the memory load

		// Update the UI in the main thread

		SetWindowText(hCPU, cpuBuffer); // Set the text of the CPU window
		SetWindowText(hRAM, ramBuffer); // Set the text of the RAM window

		Sleep(1000); // Sleep for 1 second
	}
}