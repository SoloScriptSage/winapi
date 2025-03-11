# WinAPI System Monitor

## Overview
WinAPI System Monitor is a Windows application built using the Windows API (WinAPI). It provides real-time monitoring of CPU usage, memory load, and basic file operations such as saving and loading data.

## Features
- **CPU Usage Monitoring**: Displays per-core and overall CPU utilization.
- **Memory Usage Monitoring**: Shows the current memory load.
- **File Operations**: Supports saving and loading text files.
- **Customizable UI**: Includes basic UI elements like buttons, text fields, and menus.
- **Multithreaded Performance**: Uses threads to update CPU and RAM stats in real-time.

## Technologies Used
- **C++**
- **Windows API (WinAPI)**
- **Multithreading (std::thread)**
- **Process Status API (psapi.h)**
- **IP Helper API (iphlpapi.h)**

## Installation
### Prerequisites
- Windows OS
- Microsoft Visual Studio (or any C++ compiler supporting WinAPI)

### Build Instructions
1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/winapi-system-monitor.git
   cd winapi-system-monitor
   ```
2. Open the project in Visual Studio.
3. Build the project (`Ctrl + Shift + B`).
4. Run the application.

## Usage
- **Monitoring**: CPU and RAM usage are displayed in real-time.
- **File Operations**:
  - Open a text file to load content.
  - Save current text input to a file.
- **UI Controls**:
  - Buttons for clearing input and reading text fields.
  - Menu options for opening, saving, and exiting the application.

## Code Structure
- **software-main.cpp**: Main application logic, window handling, and monitoring functions.
- **software-definitions.h**: Contains constants, function prototypes, and global variables.
- **resource.h**: Defines resource identifiers for UI components.

## Contributing
Contributions are welcome! Feel free to submit issues or pull requests.

## License
This project is licensed under the MIT License.

## Contact
For any inquiries, contact [your email] or visit [your GitHub profile].

