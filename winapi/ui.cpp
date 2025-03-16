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
	rc = { 310, 380, 10, 415 }; // x2, y1, x1, y2

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