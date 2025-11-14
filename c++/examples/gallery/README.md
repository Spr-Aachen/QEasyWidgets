# QEasyWidgets Showcase Application

A comprehensive example application demonstrating the capabilities of the QEasyWidgets C++ component library.


## Overview

This gallery application provides a beautiful, interactive demonstration of various QEasyWidgets components including:

- **Buttons**: Primary, Standard, Transparent, and Disabled states
- **Input Widgets**: Line edits, text edits, with clear buttons and validation
- **Components**: Labels, CheckBoxes, ComboBoxes with theme support
- **Interactive Demos**: Real-time counter and component interactions
- **Themed Windows**: Modern frameless windows with light/dark theme support


## Application Structure

```
gallery/
├── main.cpp              # Main application source
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```


## Building the Example

### Prerequisites

- CMake 3.16 or higher
- Qt 5.15 or Qt 6.x
- C++17 compatible compiler
- QEasyWidgets library (automatically included)

### Build Instructions

- Using CMake (Recommended)
    ```bash
    # Navigate to the gallery directory
    cd c++/examples/gallery

    # Create build directory
    mkdir build
    cd build

    # Configure
    cmake ..

    # Build
    cmake --build .

    # Run (the executable will be in c++/bin/)
    ../../bin/gallery
    ```

- Build from examples root
    ```bash
    # Navigate to examples directory
    cd c++/examples

    # Create build directory
    mkdir build
    cd build

    # Configure
    cmake ../gallery

    # Build
    cmake --build .

    # Run
    ../bin/gallery
    ```