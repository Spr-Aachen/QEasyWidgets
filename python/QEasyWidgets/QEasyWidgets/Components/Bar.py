from PySide6.QtCore import Qt, QSize
from PySide6.QtGui import QColor, QMouseEvent
from PySide6.QtWidgets import QWidget, QMainWindow, QDialog, QDockWidget, QLabel, QHBoxLayout, QSizePolicy

from ..Common.Icon import IconBase
from ..Common.StyleSheet import StyleSheetBase
from .Button import ButtonBase
from .Label import LabelBase

##############################################################################################################################

class TitleBarBase(QWidget):
    """
    Base class for titleBar components
    """
    DEFAULT_TITILE_BAR_HEIGHT = 30 # 默认标题栏高度

    _closeButton = None
    _maximizeButton = None
    _minimizeButton = None

    def __init__(self, parent: QWidget = ...,):
        super().__init__(parent)

        self.Window = parent if isinstance(parent, (QMainWindow, QDialog, QDockWidget)) else parent.window()
        #self.Window.installEventFilter(self)

        self.setFixedHeight(self.DEFAULT_TITILE_BAR_HEIGHT)
        self.setGeometry(0, 0, self.Window.width(), self.height())

        layout = QHBoxLayout(self)
        layout.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        layout.addStretch(1)

        iconSize = QSize(self.DEFAULT_TITILE_BAR_HEIGHT//2, self.DEFAULT_TITILE_BAR_HEIGHT//2)
        self.closeButton.setIconSize(iconSize)
        self.maximizeButton.setIconSize(iconSize)
        self.minimizeButton.setIconSize(iconSize)

        StyleSheetBase.Bar.apply(self)

    def _closeEvent(self):
        self.Window.close()

    def _maximizeEvent(self):
        self.Window.showNormal() if self.Window.isMaximized() else self.Window.showMaximized()
        self.maximizeButton.setIcon(IconBase.FullScreen_Exit if self.Window.isMaximized() else IconBase.FullScreen)

    def _minimizeEvent(self):
        self.Window.showMinimized()

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        if 0 < event.position().y() < self.height() and event.buttons() == Qt.MouseButton.LeftButton:
            self._maximizeEvent()

    @property
    def closeButton(self):
        if self._closeButton is None:
            self.closeButton = ButtonBase(self)
        return self._closeButton

    @closeButton.setter
    def closeButton(self, button: ButtonBase):
        button.setBorderless(True)
        button.setTransparent(True)
        button.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        button.setHoverBackgroundColor(QColor(210, 123, 123, 210))
        button.setIcon(IconBase.X)
        button.setCursor(Qt.PointingHandCursor)
        button.clicked.connect(self._closeEvent)
        index = self.layout().count()
        self.layout().insertWidget(index, button, stretch = 0, alignment = Qt.AlignRight)
        self._closeButton = button

    @property
    def maximizeButton(self):
        if self._maximizeButton is None:
            self.maximizeButton = ButtonBase(self)
        return self._maximizeButton

    @maximizeButton.setter
    def maximizeButton(self, button: ButtonBase):
        button.setBorderless(True)
        button.setTransparent(True)
        button.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        button.setHoverBackgroundColor(QColor(123, 123, 123, 123))
        button.setIcon(IconBase.FullScreen)
        button.setCursor(Qt.PointingHandCursor)
        button.clicked.connect(self._maximizeEvent)
        index = self.layout().indexOf(self.closeButton)
        self.layout().insertWidget(index, button, stretch = 0, alignment = Qt.AlignRight)
        self._maximizeButton = button

    @property
    def minimizeButton(self):
        if self._minimizeButton is None:
            self.minimizeButton = ButtonBase(self)
        return self._minimizeButton

    @minimizeButton.setter
    def minimizeButton(self, button: ButtonBase):
        button.setBorderless(True)
        button.setTransparent(True)
        button.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        button.setHoverBackgroundColor(QColor(123, 123, 123, 123))
        button.setIcon(IconBase.Dash)
        button.setCursor(Qt.PointingHandCursor)
        button.clicked.connect(self._minimizeEvent)
        index = self.layout().indexOf(self.maximizeButton)
        self.layout().insertWidget(index, button, stretch = 0, alignment = Qt.AlignRight)
        self._minimizeButton = button

    def setTitle(self, Text: str, parent):
        titleLabel = LabelBase(parent)
        titleLabel.setText(Text)
        titleLabel.setGeometry(0 + 33, 0 + self.height() / 5, self.width() / 2, self.height())
        #titleLabel.setFont(QFont("Microsoft YaHei", 11.1, QFont.Normal))
        titleLabel.setAlignment(Qt.AlignLeft | Qt.AlignVCenter)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Bar.deregistrate(self)

##############################################################################################################################