from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Widget import SizableWidget

##############################################################################################################################

class FrameBase(QFrame, SizableWidget):
    """
    Base class for frame components
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