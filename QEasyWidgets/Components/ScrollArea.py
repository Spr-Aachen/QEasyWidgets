from typing import Optional, overload
#from functools import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class ScrollBar(QScrollBar):
    '''
    '''
    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None)-> None:
        super().__init__(parent)

    @__init__.register
    def _(self, arg__1: Qt.Orientation, parent: Optional[QWidget] = ...) -> None:
        self.__init__(parent)
        self.setOrientation(arg__1)


class ScrollAreaBase(QScrollArea):
    """
    Base class for scrollArea components
    """
    viewportSizeChanged = Signal(QSize)

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setWidgetResizable(True)

        StyleSheetBase.ScrollArea.Apply(self)

    def setWidget(self, widget: QWidget) -> None:
        self.widget().deleteLater() if self.widget() is not None else None
        super().setWidget(widget)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.viewportSizeChanged.emit(self.viewport().size())
        super().resizeEvent(event)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ScrollArea.Deregistrate(self)


class VerticalScrollArea(ScrollAreaBase):
    """
    Vertical scrollArea component
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.viewportSizeChanged.connect(self.onViewportSizeChanged)

    def onViewportSizeChanged(self, size: QSize) -> None:
        self.widget().setMaximumWidth(size.width()) if self.widget() is not None else None

##############################################################################################################################