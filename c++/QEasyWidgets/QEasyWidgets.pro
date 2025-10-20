QT += widgets svg xml

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# Disable deprecated APIs
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

#-------------------------------------------------------------------------------
# Compiler options
#-------------------------------------------------------------------------------

*g++*: {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE *= -O3
}

*msvc*: {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
    QMAKE_CXXFLAGS_RELEASE -= /O
    QMAKE_CXXFLAGS_RELEASE *= /O2
}

CONFIG(debug, release|debug){
    win32:TARGET = qeasywidgetsd
    else:TARGET = qeasywidgets
} else {
    TARGET = qeasywidgets
}

#-------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------

SOURCES += \
    QEasyWidgets.cpp \
    Common/Config.cpp \
    Common/Icon.cpp \
    Common/Language.cpp \
    Common/QFunctions.cpp \
    Common/QTasks.cpp \
    Common/QWorker.cpp \
    Common/Signals.cpp \
    Common/StyleSheet.cpp \
    Common/Theme.cpp \
    Common/Translator.cpp \
    Components/Bar.cpp \
    Components/Browser.cpp \
    Components/Button.cpp \
    Components/ChatWidget.cpp \
    Components/CheckBox.cpp \
    Components/ComboBox.cpp \
    Components/DockWidget.cpp \
    Components/Edit.cpp \
    Components/Frame.cpp \
    Components/GroupBox.cpp \
    Components/Label.cpp \
    Components/List.cpp \
    Components/Menu.cpp \
    Components/Player.cpp \
    Components/ProgressBar.cpp \
    Components/ScrollArea.cpp \
    Components/ScrollBar.cpp \
    Components/Slider.cpp \
    Components/SpinBox.cpp \
    Components/StatusWidget.cpp \
    Components/Tab.cpp \
    Components/Table.cpp \
    Components/ToolBox.cpp \
    Components/Tree.cpp \
    Components/Widget.cpp \
    Windows/FramelessWindow/FramelessWindow.cpp \
    Windows/Window.cpp \
    Windows/Dialog.cpp

HEADERS += \
    QEasyWidgets.h \
    QEasyWidgets_All.h \
    Common/Config.h \
    Common/Icon.h \
    Common/Language.h \
    Common/QFunctions.h \
    Common/QTasks.h \
    Common/QWorker.h \
    Common/Signals.h \
    Common/StyleSheet.h \
    Common/Theme.h \
    Common/Translator.h \
    Components/Bar.h \
    Components/Browser.h \
    Components/Button.h \
    Components/ChatWidget.h \
    Components/CheckBox.h \
    Components/ComboBox.h \
    Components/DockWidget.h \
    Components/Edit.h \
    Components/Frame.h \
    Components/GroupBox.h \
    Components/Label.h \
    Components/List.h \
    Components/Menu.h \
    Components/Player.h \
    Components/ProgressBar.h \
    Components/ScrollArea.h \
    Components/ScrollBar.h \
    Components/Slider.h \
    Components/SpinBox.h \
    Components/StatusWidget.h \
    Components/Tab.h \
    Components/Table.h \
    Components/ToolBox.h \
    Components/Tree.h \
    Components/Widget.h \
    Windows/FramelessWindow/FramelessWindow.h \
    Windows/Window.h \
    Windows/Dialog.h

#-------------------------------------------------------------------------------
# Build options
#-------------------------------------------------------------------------------

DESTDIR = $$PWD/../bin
LIBS += -L$$PWD/../bin

# Default rules for deployment
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

# Resources
RESOURCES += \
    Resources/Sources.qrc
