#ifndef SOFTWARE_DEFINITIONS_H
#define SOFTWARE_DEFINITIONS_H

#define SAVE_FILE 1
#define LOAD_FILE 2

#define TEXT_BUFFER_SIZE	256
#define DIGIT_INDEX_NUMBER 200

#define BTN_CLICK_ME 1001  // Button ID
#define BTN_EXIT     1002  // Button ID
#define BTN_CLS	  1003  // Button ID
#define BTN_READ	1004
#define BTN_READ_CLR 1005

#define MENU_NEW	2001  // Menu item ID for "New"
#define MENU_OPEN	2002  // Menu item ID for "Open"
#define MENU_SAVE	2003  // Menu item ID for "Save"

#define MENU_EXIT	2004  // Menu item ID for "Exit"
#define MENU_CUT	2005  // Menu item ID for "Cut"
#define MENU_COPY	2006  // Menu item ID for "Copy"
#define MENU_PASTE	2007  // Menu item ID for "Paste
#define MENU_HELP	2008  // Menu item ID for "Help"
#define MENU_ABOUT	2009  // Menu item ID for "About"
#define MENU_PRJCT	2010  // Menu item ID for "Project"
#define MENU_FILE	2011  // Menu item ID for "File"  
#define MENU_REPO	2012  // Menu item ID for "Repository"

#define MAX_PATH 260

#define DLG_INDEX_COLOR_R 201
#define DLG_INDEX_COLOR_G 202
#define DLG_INDEX_COLOR_B 203

HFONT fontRectangle; // Font
COLORREF fontColor; // Color
int clrR = 0, clrG = 0, clrB = 0; // RGB values

HBRUSH brushRectangle; // Background color
PAINTSTRUCT ps; // Paint structure
RECT rc; // Rectangle structure

char Buffer[TEXT_BUFFER_SIZE];
int readChars;

unsigned num = 0;

HWND hNumberControl;
HWND hStaticControl;
HWND hEditControl; // Handle to the edit control


char filename[MAX_PATH]; // File name
OPENFILENAMEA ofn; // Open file dialog structure

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


void PrintPerCoreCPUUsage(); // Function to print the CPU usage per core
void GetMemoryUsage(HWND hWND); // Function to get the memory usage

void GetDiskUsage(); // Function to get the disk usage (future development)
void GetNetworkUsage(); //	Function to get the network usage (future development)

void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParameters(HWND hWND);

#endif // SOFTWARE_DEFINITIONS_H