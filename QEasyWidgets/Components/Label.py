from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class LabelBase(QLabel):
    '''
    '''
    resized = Signal()

    _pixmap = None

    def __init__(self, parent: Optional[QWidget] = None, f: Qt.WindowType = ...) -> None:
        super().__init__(parent)

        StyleSheetBase.Label.Apply(self)

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

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Label.Deregistrate(self)

##############################################################################################################################