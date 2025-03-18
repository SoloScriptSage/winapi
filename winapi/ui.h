// ui.h
#ifndef UI_H
#define UI_H

#include <windows.h>

// Global variables for UI controls
extern HWND hStaticControl;
extern HWND hEditControl;
extern HWND hNumberControl;
extern HWND hCPU;
extern HWND hRAM;
extern HWND hDisk;
extern HWND hNetwork;
extern RECT rc;

#define DIGIT_INDEX_NUMBER 200

#define DLG_INDEX_COLOR_R 201
#define DLG_INDEX_COLOR_G 202
#define DLG_INDEX_COLOR_B 203

#define BTN_READ_CLR 1005

// Function declarations
void MainWndAddMenus(HWND hWnd);
void MainWndAddWidgets(HWND hWnd);

#endif // UI_H
