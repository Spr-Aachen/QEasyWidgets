from PySide6.QtCore import Qt
from PySide6.QtWidgets import QWidget, QMainWindow, QDialog, QLabel, QHBoxLayout, QSizePolicy

from ...Common.Icon import IconBase
from ...Common.StyleSheet import StyleSheetBase
from ...Components.Button import ButtonBase

##############################################################################################################################

class TitleBarBase(QWidget):
    '''
    '''
    DEFAULT_TITILE_BAR_HEIGHT = 30 # 默认标题栏高度

    def __init__(self,
        parent: QWidget = ...,
    ):
        super().__init__(parent)

        self.Window = parent if isinstance(parent, (QMainWindow, QDialog)) else parent.window()
        self.Window.installEventFilter(self)

        self.setFixedHeight(self.DEFAULT_TITILE_BAR_HEIGHT)
        self.setGeometry(0, 0, self.Window.width(), self.height())

        self.MinimizeButton = self.setMinimizeButton(parent = self)
        self.MinimizeButton.clicked.connect(self.setMinimizeEvent)
        self.MaximizeButton = self.setMaximizeButton(parent = self)
        self.MaximizeButton.clicked.connect(self.setMaximizeEvent)
        self.CloseButton = self.setCloseButton(parent = self)
        self.CloseButton.clicked.connect(self.setCloseEvent)

        self.HBoxLayout = QHBoxLayout(self)
        self.HBoxLayout.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        self.HBoxLayout.setContentsMargins(0, 0, 0, 0)
        self.HBoxLayout.setSpacing(0)
        self.HBoxLayout.addStretch(1)
        self.HBoxLayout.addWidget(self.MinimizeButton, stretch = 0, alignment = Qt.AlignRight)
        self.HBoxLayout.addWidget(self.MaximizeButton, stretch = 0, alignment = Qt.AlignRight)
        self.HBoxLayout.addWidget(self.CloseButton, stretch = 0, alignment = Qt.AlignRight)

        StyleSheetBase.Bar.Apply(self)

    '''
    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        if 0 < event.position().y() < self.height() and event.buttons() == Qt.MouseButton.LeftButton:
            self.setMaximizeEvent()
    '''

    def setCloseEvent(self):
        self.Window.close()

    def setMaximizeEvent(self):
        self.Window.showNormal() if self.Window.isMaximized() else self.Window.showMaximized()

    def setMinimizeEvent(self):
        self.Window.showMinimized()

    def setCloseButton(self, parent):
        CloseButton = ButtonBase(parent)
        CloseButton.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        CloseButton.ClearDefaultStyleSheet()
        CloseButton.setStyleSheet(
            "ButtonBase {"
            "   background-color: transparent;"
            "}"
            "ButtonBase:hover {"
            "   background-color: rgba(210, 123, 123, 210);"
            "}"
        )
        CloseButton.setIcon(IconBase.X)
        CloseButton.setCursor(Qt.PointingHandCursor)
        return CloseButton

    def setMaximizeButton(self, parent):
        MaximizeButton = ButtonBase(parent)
        MaximizeButton.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        MaximizeButton.ClearDefaultStyleSheet()
        MaximizeButton.setStyleSheet(
            "ButtonBase {"
            "   background-color: transparent;"
            "}"
            "ButtonBase:hover {"
            "   background-color: rgba(123, 123, 123, 123);"
            "}"
        )
        MaximizeButton.setIcon(IconBase.FullScreen)
        MaximizeButton.setCursor(Qt.PointingHandCursor)
        return MaximizeButton

    def setMinimizeButton(self, parent):
        MinimizeButton = ButtonBase(parent)
        MinimizeButton.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        MinimizeButton.ClearDefaultStyleSheet()
        MinimizeButton.setStyleSheet(
            "ButtonBase {"
            "   background-color: transparent;"
            "}"
            "ButtonBase:hover {"
            "   background-color: rgba(123, 123, 123, 123);"
            "}"
        )
        MinimizeButton.setIcon(IconBase.Dash)
        MinimizeButton.setCursor(Qt.PointingHandCursor)
        return MinimizeButton

    def setTitle(self, Text: str, parent):
        TitleLabel = QLabel(Text, parent)
        TitleLabel.setGeometry(0 + 33, 0 + self.height() / 5, self.width() / 2, self.height())
        TitleLabel.setStyleSheet(
            "QLabel {"
            "   color: rgba(210, 210, 210, 210);"
            "   background-color: transparent;"
            "   padding: 3.3px;"
            "   border-width: 0px;"
            "   border-style: solid;"
            "}"
        )
        #TitleLabel.setFont(QFont("Microsoft YaHei", 11.1, QFont.Normal))
        TitleLabel.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Bar.Deregistrate(self)

##############################################################################################################################