from typing import Optional, overload
#from functools import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Bar import TitleBarBase
from .Button import ButtonBase

##############################################################################################################################

class DockTitleBar(TitleBarBase):
    """
    Custom dockWidget titleBar
    """
    _floatButton = None

    def __init__(self, parent: QDockWidget = ...) -> None:
        super().__init__(parent)

        self.setFixedHeight(self.DEFAULT_TITILE_BAR_HEIGHT//2)

        iconSize = QSize(self.DEFAULT_TITILE_BAR_HEIGHT//4, self.DEFAULT_TITILE_BAR_HEIGHT//4)
        self.closeButton.setIconSize(iconSize)
        self.maximizeButton.deleteLater()
        self.floatButton.setIconSize(iconSize)
        self.minimizeButton.setIconSize(iconSize)

    def _toggleFloating(self):
        dock: QDockWidget = self.parent()
        dock.setFloating(not dock.isFloating())
        self.floatButton.setIcon(IconBase.Window_FullScreen if dock.isFloating() else IconBase.Window_Stack)

    @property
    def floatButton(self):
        if self._floatButton is None:
            self.floatButton = ButtonBase(self)
        return self._floatButton

    @floatButton.setter
    def floatButton(self, button: ButtonBase):
        button.setBorderless(True)
        button.setTransparent(True)
        button.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding))
        button.setHoverBackgroundColor(QColor(123, 123, 123, 123))
        button.setIcon(IconBase.Window_FullScreen)
        button.setCursor(Qt.PointingHandCursor)
        button.clicked.connect(self._toggleFloating)
        index = self.layout().indexOf(self.closeButton) - 1
        self.layout().insertWidget(index, button, stretch = 0, alignment = Qt.AlignRight)
        self._floatButton = button


class DockWidgetBase(QDockWidget):
    """
    Base class for dockWidget components
    """
    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        titleBar = DockTitleBar(self)
        self.setTitleBarWidget(titleBar)

        StyleSheetBase.DockWidget.Apply(self)

    @__init__.register
    def _(self, title: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setWindowTitle(title)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.DockWidget.Deregistrate(self)

##############################################################################################################################