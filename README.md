<div align = "center">

# QEasyWidgets

A modern Qt widget library that provides enhanced components with theme support, animations, and a clean API.

[![PyPI](https://img.shields.io/pypi/v/QEasyWidgets?color=blue&logo=PYPY&logoColor=blue&style=for-the-badge)](https://pypi.org/project/QEasyWidgets/)&nbsp;

[**简体中文**](./docs/README_CN.md) | **English**

![Title](./docs/media/Title.png)

</div>



# Python


## Deployment

### pip

```shell
pip install QEasyWidgets -i https://pypi.org/simple/
```



# C++


## Building

### Requirements

- Qt 5.15+ or Qt 6.x
- C++17 compatible compiler
- qmake or CMake

### Using qmake

```bash
cd QEasyWidgets
qmake QEasyWidgets.pro
make          # Linux/macOS
nmake         # Windows (MSVC)
mingw32-make  # Windows (MinGW)
```

### Using CMake

```bash
cd QEasyWidgets
mkdir build && cd build
cmake ..
cmake --build .
```

### Build Output

The library will be built as a static library:
- Debug: `qeasywidgetsd.lib` (Windows) or `libqeasywidgets.a` (Unix)
- Release: `qeasywidgets.lib` (Windows) or `libqeasywidgets.a` (Unix)


## Usage

### Basic Example

```cpp
#include <QApplication>
#include "QEasyWidgets.h"
#include "Components/Button.h"
#include "Windows/Window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Initialize QEasyWidgets
    QEWIns.setTheme(QEW::LIGHT);
    
    // Create a window
    QEW::WindowBase window;
    
    // Create a button
    QEW::PrimaryButton *button = new QEW::PrimaryButton("Click Me", &window);
    button->setIcon(QEW::IconBase::Play);
    
    window.setCentralWidget(button);
    window.show();
    
    return app.exec();
}
```

### Theme Management

```cpp
// Set theme
QEWIns.setTheme(QEW::DARK);

// Set custom theme color
QEWIns.setThemeColor(QColor(0, 120, 215));

// Check current theme
if (QEWIns.isDarkTheme()) {
    // Do something for dark theme
}

// Listen to theme changes
QObject::connect(&QEWIns, &QEasyWidgets::themeChanged, [](QEW::Theme theme) {
    qDebug() << "Theme changed to:" << QEW::ThemeString(theme);
});
```

### Using Components

#### Buttons

```cpp
// Basic button
QEW::ButtonBase *button = new QEW::ButtonBase("Button", parent);

// Primary button (accent color)
QEW::PrimaryButton *primaryBtn = new QEW::PrimaryButton("Primary", parent);

// Transparent button
QEW::TransparentButton *transBtn = new QEW::TransparentButton("Transparent", parent);

// Button with icon
button->setIcon(QEW::IconBase::Download);
button->setSpacing(5);
```

#### Line Edit

```cpp
QEW::LineEditBase *lineEdit = new QEW::LineEditBase(parent);
lineEdit->setClearButtonEnabled(true);
lineEdit->setPlaceholderText("Enter text...");
```

#### Dialogs

```cpp
QEW::MessageDialog dialog("Title", "Content", parent);
dialog.setOkButtonText("Confirm");
dialog.setCancelButtonText("Cancel");

if (dialog.exec() == QDialog::Accepted) {
    // User clicked OK
}
```

### Custom Configuration Path

```cpp
// Set custom config file location
QEWIns.setConfigPath("/path/to/config.ini");
```


## Namespace

All classes are in the `QEW` namespace (QEasyWidgets), except for the main `QEasyWidgets` singleton class.

## Icons

The library supports the following built-in icons (IconBase enum):

- Arrow_Clockwise, Arrow_Repeat
- Chevron_Left, Chevron_Right, Chevron_Up, Chevron_Down
- CompactChevron_Left, CompactChevron_Right
- Ellipsis, OpenedFolder, Clipboard
- Download, Play, Pause, Send, Stop
- Dash, Window_FullScreen, Window_Stack
- FullScreen, FullScreen_Exit, X

## Extending

### Adding New Components

1. Create header and source files in the `Components/` directory
2. Inherit from appropriate Qt base class
3. Apply theme support using `StyleSheetBase`
4. Add files to `QEasyWidgets.pro`

### Adding Custom Stylesheets

1. Create `.qss` files in `qss/light/` and `qss/dark/` directories
2. Add to `qeasywidgets.qrc`
3. Load using `StyleSheetBase::loadStyleSheet()`



# Cases
Here are some projects based on QEasyWidgets:
- [Easy Voice Toolkit](https://github.com/Spr-Aachen/Easy-Voice-Toolkit)
- [LLM PromptMaster](https://github.com/Spr-Aachen/LLM-PromptMaster)
- [ParkingLot-Management](https://github.com/Spr-Aachen/ParkingLot-Management)



# Reference
- [Microsoft WinUI-Gallery](https://github.com/microsoft/WinUI-Gallery)
- [PyEasyUtils](https://github.com/Spr-Aachen/PyEasyUtils)