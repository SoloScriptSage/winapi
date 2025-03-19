#include "main.h" // Include the software definitions header file

#include "ui.h"
#include "cpu_monitor.h"
#include "ram_monitor.h"
#include "disk_monitor.h"
#include "network_monitor.h"
#include "gpu_monitor.h"
#include "alerts.h"
#include "globals.h"

#pragma comment(lib, "iphlpapi.lib") // Link the iphlpapi library

#define WM_START_THREADS (WM_USER + 1)

using namespace std;
using namespace chrono;

thread cpuThread; // Thread for updating CPU usage
thread ramThread; // Thread for updating RAM usage
thread diskThread; // Thread for updating disk usage
thread networkThread; // Thread for updating network usage


void AlertThread() {
	while (updateFlag) {
		CheckAndShowAlerts(); // Check and show alerts
		this_thread::sleep_for(1s); // Sleep for 1 second
	}
}

thread alertThread(AlertThread); // Thread for showing alerts

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

