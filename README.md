<div align = "center">

# QEasyWidgets

A modern Qt widget library that provides enhanced components with theme support, animations, and a clean API.

[![PyPI](https://img.shields.io/pypi/v/QEasyWidgets?color=blue&logo=PYPY&logoColor=blue&style=for-the-badge)](https://pypi.org/project/QEasyWidgets/)&nbsp;

[**简体中文**](./docs/README_CN.md) | **English**

![Title](./docs/media/Title.png)

</div>

---

## Python

### Deployment

- pip
    ```shell
    pip install QEasyWidgets -i https://pypi.org/simple/
    ```


## C++

### Requirements

- Qt 6.x or Qt 5.15+
- C++17 compatible compiler
- qmake or CMake

### Build Command

- Using qmake
```bash
cd QEasyWidgets
qmake QEasyWidgets.pro
make          # Linux/macOS
nmake         # Windows (MSVC)
mingw32-make  # Windows (MinGW)
```

- Using CMake
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

### Usage

- Basic Example
```cpp
#include <QApplication>

#include "QEasyWidgets.h"
#include "Components/Button.h"
#include "Windows/Window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QEWIns.setTheme(LIGHT); // Initialize QEasyWidgets
    WindowBase window; // Create a window
    PrimaryButton *button = new PrimaryButton("Click Me", &window); // Create a button
    button->setIcon(IconBase::Play);
    window.setCentralWidget(button);
    window.show();
    return app.exec();
}
```


## Cases
Here are some projects based on QEasyWidgets:
- [Easy Voice Toolkit](https://github.com/Spr-Aachen/Easy-Voice-Toolkit)
- [LLM PromptMaster](https://github.com/Spr-Aachen/LLM-PromptMaster)
- [ParkingLot-Management](https://github.com/Spr-Aachen/ParkingLot-Management)


## Reference
- [Microsoft WinUI-Gallery](https://github.com/microsoft/WinUI-Gallery)
- [PyEasyUtils](https://github.com/Spr-Aachen/PyEasyUtils)