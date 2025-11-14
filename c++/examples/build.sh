#!/bin/bash
# Build script for QEasyWidgets Examples (Linux/macOS)

echo "========================================"
echo "QEasyWidgets Examples Build Script"
echo "========================================"
echo ""

# Check if build directory exists
if [ -d "build" ]; then
    echo "Build directory exists. Cleaning..."
    rm -rf build
fi

# Create build directory
echo "Creating build directory..."
mkdir build
cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
echo ""

if [ -n "$Qt6_DIR" ]; then
    echo "Using Qt6_DIR: $Qt6_DIR"
    cmake .. -DCMAKE_PREFIX_PATH="$Qt6_DIR"
elif [ -n "$Qt5_DIR" ]; then
    echo "Using Qt5_DIR: $Qt5_DIR"
    cmake .. -DCMAKE_PREFIX_PATH="$Qt5_DIR"
else
    echo "No Qt path specified. Attempting automatic detection..."
    cmake ..
fi

if [ $? -ne 0 ]; then
    echo ""
    echo "========================================"
    echo "ERROR: CMake configuration failed!"
    echo "========================================"
    echo ""
    echo "Please set Qt6_DIR or Qt5_DIR environment variable:"
    echo "  export Qt6_DIR=/path/to/Qt/6.x.x/gcc_64"
    echo ""
    echo "Or specify it when running cmake:"
    echo "  cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64"
    echo ""
    exit 1
fi

# Build
echo ""
echo "Building..."
echo ""

# Detect number of CPU cores
if [ "$(uname)" == "Darwin" ]; then
    # macOS
    CORES=$(sysctl -n hw.ncpu)
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Linux
    CORES=$(nproc)
else
    CORES=2
fi

cmake --build . -j$CORES

if [ $? -ne 0 ]; then
    echo ""
    echo "========================================"
    echo "ERROR: Build failed!"
    echo "========================================"
    echo ""
    exit 1
fi

# Success
echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "Executable location: ../bin/gallery"
echo ""
echo "To run the gallery:"
echo "  cd ../bin"
echo "  ./gallery"
echo ""