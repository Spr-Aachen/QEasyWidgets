<div align = "center">

# QEasyWidgets

一个现代化 Qt 组件库，提供了增强的组件，支持主题切换、动画效果和简洁的 API。

[![PyPI](https://img.shields.io/pypi/v/QEasyWidgets?color=blue&logo=PYPY&logoColor=blue&style=for-the-badge)](https://pypi.org/project/QEasyWidgets/)&nbsp;

**简体中文** | [**English**](../README.md)

![Title](./media/Title.png)

</div>



# Python


## 部署

### pip

```shell
pip install QEasyWidgets -i https://pypi.org/simple/
```



# C++


## 构建

### 要求

- Qt 6.x 或 Qt 5.15+
- 支持 C++17 的编译器
- qmake 或 CMake

### 使用 qmake

```bash
cd QEasyWidgets
qmake QEasyWidgets.pro
make          # Linux/macOS
nmake         # Windows (MSVC)
mingw32-make  # Windows (MinGW)
```

### 使用 CMake

```bash
cd QEasyWidgets
mkdir build && cd build
cmake ..
cmake --build .
```

### 构建输出

库文件将构建为静态库：
- **调试版**: `qeasywidgetsd.lib` (Windows) 或 `libqeasywidgets.a` (Unix)
- **发布版**: `qeasywidgets.lib` (Windows) 或 `libqeasywidgets.a` (Unix)


## 使用方法

### 基础示例

```cpp
#include <QApplication>
#include "QEasyWidgets.h"
#include "Components/Button.h"
#include "Windows/Window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 初始化 QEasyWidgets
    QEWIns.setTheme(QEW::LIGHT);
    
    // 创建窗口
    QEW::WindowBase window;
    
    // 创建按钮
    QEW::PrimaryButton *button = new QEW::PrimaryButton("点击我", &window);
    button->setIcon(QEW::IconBase::Play);
    
    window.setCentralWidget(button);
    window.show();
    
    return app.exec();
}
```

### 主题管理

```cpp
// 设置主题
QEWIns.setTheme(QEW::DARK);

// 设置自定义主题颜色
QEWIns.setThemeColor(QColor(0, 120, 215));

// 检查当前主题
if (QEWIns.isDarkTheme()) {
    // 暗色主题相关操作
}

// 监听主题变化
QObject::connect(&QEWIns, &QEasyWidgets::themeChanged, [](QEW::Theme theme) {
    qDebug() << "主题已更改为:" << QEW::ThemeString(theme);
});
```

### 使用组件

#### 按钮

```cpp
// 基础按钮
QEW::ButtonBase *button = new QEW::ButtonBase("按钮", parent);

// 主要按钮（强调色）
QEW::PrimaryButton *primaryBtn = new QEW::PrimaryButton("主要", parent);

// 透明按钮
QEW::TransparentButton *transBtn = new QEW::TransparentButton("透明", parent);

// 带图标的按钮
button->setIcon(QEW::IconBase::Download);
button->setSpacing(5);
```

#### 输入框

```cpp
QEW::LineEditBase *lineEdit = new QEW::LineEditBase(parent);
lineEdit->setClearButtonEnabled(true);
lineEdit->setPlaceholderText("请输入文本...");
```

#### 对话框

```cpp
QEW::MessageDialog dialog("标题", "内容", parent);
dialog.setOkButtonText("确认");
dialog.setCancelButtonText("取消");

if (dialog.exec() == QDialog::Accepted) {
    // 用户点击了确认
}
```

### 自定义配置路径

```cpp
// 设置自定义配置文件位置
QEWIns.setConfigPath("/path/to/config.ini");
```


## 命名空间

所有类都在 `QEW` 命名空间中（QEasyWidgets），主 `QEasyWidgets` 单例类除外。

## 图标

库支持以下内置图标（IconBase 枚举）：

- Arrow_Clockwise, Arrow_Repeat
- Chevron_Left, Chevron_Right, Chevron_Up, Chevron_Down
- CompactChevron_Left, CompactChevron_Right
- Ellipsis, OpenedFolder, Clipboard
- Download, Play, Pause, Send, Stop
- Dash, Window_FullScreen, Window_Stack
- FullScreen, FullScreen_Exit, X

## 扩展

### 添加新组件

1. 在 `Components/` 目录中创建头文件和源文件
2. 继承适当的 Qt 基类
3. 使用 `StyleSheetBase` 应用主题支持
4. 将文件添加到 `QEasyWidgets.pro`

### 添加自定义样式表

1. 在 `qss/light/` 和 `qss/dark/` 目录中创建 `.qss` 文件
2. 添加到 `source.qrc`
3. 使用 `StyleSheetBase::loadStyleSheet()` 加载



# 案例
这里有些基于本组件库的项目:
- [Easy Voice Toolkit](https://github.com/Spr-Aachen/Easy-Voice-Toolkit)
- [LLM PromptMaster](https://github.com/Spr-Aachen/LLM-PromptMaster)
- [ParkingLot-Management](https://github.com/Spr-Aachen/ParkingLot-Management)



# 指路
- [Microsoft WinUI-Gallery](https://github.com/microsoft/WinUI-Gallery)
- [PyEasyUtils](https://github.com/Spr-Aachen/PyEasyUtils)