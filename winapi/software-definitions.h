#pragma once

#define OnMenuClicked 1
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