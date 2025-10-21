cmake_minimum_required(VERSION 3.16)

project(QEasyWidgets VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Core Widgets Svg Xml QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Widgets Svg Xml)
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

# Source files
set(SOURCES
    QEasyWidgets.cpp
    Common/Config.cpp
    Common/Icon.cpp
    Common/Language.cpp
    Common/QFunctions.cpp
    Common/QTasks.cpp
    Common/QWorker.cpp
    Common/Signals.cpp
    Common/StyleSheet.cpp
    Common/Theme.cpp
    Common/Translator.cpp
    Components/Bar.cpp
    Components/Browser.cpp
    Components/Button.cpp
    Components/ChatWidget.cpp
    Components/CheckBox.cpp
    Components/ComboBox.cpp
    Components/DockWidget.cpp
    Components/Edit.cpp
    Components/Frame.cpp
    Components/GroupBox.cpp
    Components/Label.cpp
    Components/List.cpp
    Components/Menu.cpp
    Components/Player.cpp
    Components/ProgressBar.cpp
    Components/ScrollArea.cpp
    Components/ScrollBar.cpp
    Components/Slider.cpp
    Components/SpinBox.cpp
    Components/StatusWidget.cpp
    Components/Tab.cpp
    Components/Table.cpp
    Components/ToolBox.cpp
    Components/Tree.cpp
    Components/Widget.cpp
    Windows/FramelessWindow/FramelessWindow.cpp
    Windows/Window.cpp
    Windows/Dialog.cpp
)

# Header files
set(HEADERS
    QEasyWidgets.h
    QEasyWidgets_All.h
    Common/Config.h
    Common/Icon.h
    Common/Language.h
    Common/QFunctions.h
    Common/QTasks.h
    Common/QWorker.h
    Common/Signals.h
    Common/StyleSheet.h
    Common/Theme.h
    Common/Translator.h
    Components/Bar.h
    Components/Browser.h
    Components/Button.h
    Components/ChatWidget.h
    Components/CheckBox.h
    Components/ComboBox.h
    Components/DockWidget.h
    Components/Edit.h
    Components/Frame.h
    Components/GroupBox.h
    Components/Label.h
    Components/List.h
    Components/Menu.h
    Components/Player.h
    Components/ProgressBar.h
    Components/ScrollArea.h
    Components/ScrollBar.h
    Components/Slider.h
    Components/SpinBox.h
    Components/StatusWidget.h
    Components/Tab.h
    Components/Table.h
    Components/ToolBox.h
    Components/Tree.h
    Components/Widget.h
    Windows/FramelessWindow/FramelessWindow.h
    Windows/Window.h
    Windows/Dialog.h
)

# Resource files
set(RESOURCES
    Resources/Sources.qrc
)

# Create static library
add_library(qeasywidgets STATIC
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Qt libraries
if (Qt6_FOUND)
    target_link_libraries(qeasywidgets PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Svg
        Qt6::Xml
    )
else()
    target_link_libraries(qeasywidgets PUBLIC
        Qt5::Core
        Qt5::Widgets
        Qt5::Svg
        Qt5::Xml
    )
endif()

# Include directories
target_include_directories(qeasywidgets PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler options
if(MSVC)
    target_compile_options(qeasywidgets PRIVATE /utf-8 /W3)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(qeasywidgets PRIVATE /O2)
    endif()
else()
    target_compile_options(qeasywidgets PRIVATE -Wall -Wextra)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(qeasywidgets PRIVATE -O3)
    endif()
endif()

# Install rules
install(TARGETS qeasywidgets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include/QEasyWidgets)
