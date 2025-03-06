#ifndef SOFTWARE_DEFINITIONS_H
#define SOFTWARE_DEFINITIONS_H

#define BTN_CLICK_ME 1001  // Button ID
#define MENU_NEW      2001  // Menu item ID for "New"
#define MENU_OPEN     2002  // Menu item ID for "Open"
#define MENU_SAVE     2003  // Menu item ID for "Save"
#define MENU_EXIT     2004  // Menu item ID for "Exit"
#define MENU_CUT      3001  // Menu item ID for "Cut"
#define MENU_COPY     3002  // Menu item ID for "Copy"
#define MENU_PASTE    3003  // Menu item ID for "Paste

// Window procedure. The function is used to process messages sent to the window (like button clicks, key presses, etc.)
// HWND hWnd: The handle to the window
// UINT uMsg: The message sent to the window
// WPARAM wParam: Additional message information
// LPARAM lParam: Additional message information
LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// This is a function declaration for creating a new window class, which defines the properties and behavior of a window
// HBRUSH BGColor: The background color of the window
// HCURSOR Cursor: The cursor of the window
// HINSTANCE hInst: The instance of the window
// HICON Icon: The icon of the window
// LPCWSTR Name: The name of the window class
// WNDPROC Procedure: The window procedure of the window
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

// This is a function declaration for adding menus to the main window
void MainWndAddMenus(HWND hWnd);

// This is a function declaration for adding widgets to the main window
void MainWndAddWidgets(HWND hWnd);


double GetCPUUsage(); // Function to get the CPU usage
void GetMemoryUsage(); // Function to get the memory usage
void GetDiskUsage(); // Function to get the disk usage
void GetNetworkUsage(); //	Function to get the network usage
DWORD WINAPI UpdateStats(LPVOID lpParam); // Function to update the system stats

#endif // SOFTWARE_DEFINITIONS_H