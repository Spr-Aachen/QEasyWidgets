cmake_minimum_required(VERSION 3.16)

project(QEasyWidgets_Gallery VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Core Widgets Svg Xml Multimedia QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets Svg Xml Multimedia)
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

# Add QEasyWidgets library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../QEasyWidgets ${CMAKE_CURRENT_BINARY_DIR}/qeasywidgets)

# Source files
set(SOURCES
    main.cpp
)

# Create executable
add_executable(gallery ${SOURCES})

# Link Qt libraries
if (Qt6_FOUND)
    target_link_libraries(qeasywidgets PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Svg
        Qt6::Xml
        Qt6::Multimedia
    )
else()
    target_link_libraries(qeasywidgets PUBLIC
        Qt5::Core
        Qt5::Widgets
        Qt5::Svg
        Qt5::Xml
        Qt5::Multimedia
    )
endif()

# Include directories
target_include_directories(gallery PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../QEasyWidgets
)

# Compiler options
if(MSVC)
    target_compile_options(gallery PRIVATE /utf-8 /W3)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(gallery PRIVATE /O2)
    endif()
else()
    target_compile_options(gallery PRIVATE -Wall -Wextra)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(gallery PRIVATE -O3)
    endif()
endif()

# Windows specific settings
if(WIN32)
    # Set application icon (if available)
    # set_target_properties(gallery PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# macOS specific settings
if(APPLE)
    set_target_properties(gallery PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.qeasywidgets.gallery"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()
