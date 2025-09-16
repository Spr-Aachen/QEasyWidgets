from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class SizableWidget:
    """
    """
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def getCurrentWidth(self):
        return getWidth(self)

    def setCurrentWidth(self, w: int):
        self.setFixedWidth(w)

    currentWidth = Property(int, getCurrentWidth, setCurrentWidth)

    def getCurrentHeight(self):
        return getHeight(self)

    def setCurrentHeight(self, h: int):
        self.setFixedHeight(h)

    currentHeight = Property(int, getCurrentHeight, setCurrentHeight)

##############################################################################################################################

class WidgetBase(QWidget, SizableWidget):
    """
    Base class for widget components
    """
    resized = Signal()

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.resized.emit()
        '''
        if self.minimumSizeHint() != self.size():
            self.adjustSize()
        '''
        super().resizeEvent(event)

##############################################################################################################################