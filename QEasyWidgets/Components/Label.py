from typing import Optional, overload
from PyEasyUtils import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class LabelBase(QLabel):
    """
    Base class for label components
    """
    resized = Signal()

    _pixmap = None

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None, f: Qt.WindowType = ...) -> None:
        super().__init__(parent)

        StyleSheetBase.Label.apply(self)

    @__init__.register
    def _(self, text: str, parent: Optional[QWidget] = None, f: Qt.WindowType = ...) -> None:
        self.__init__(parent)
        self.setText(text)

    def getCurrentWidth(self):
        return getWidth(self)

    def setCurrentWidth(self, w: int):
        self.setFixedWidth(w)

    currentWidth = Property(int, getCurrentWidth, setCurrentWidth)

    def getCurrentHeight(self):
        return getHeight(self)

    def setCurrentHeight(self, w: int):
        self.setFixedHeight(w)

    currentHeight = Property(int, getCurrentHeight, setCurrentHeight)

    def scalePixmap(self, pixmap: QPixmap):
        Length = max(self.width(), self.height())
        scaled_pixmap = pixmap.scaled(
            Length, Length,
            Qt.KeepAspectRatio, 
            Qt.SmoothTransformation
        )
        return scaled_pixmap

    def setPixmap(self, pixmap: QPixmap):
        #self.setAlignment(Qt.AlignCenter)
        self._pixmap = pixmap
        super().setPixmap(self.scalePixmap(pixmap))

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.resized.emit()
        super().setPixmap(self.scalePixmap(self._pixmap)) if not self._pixmap is None else None
        super().resizeEvent(event)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Label.deregistrate(self)

##############################################################################################################################