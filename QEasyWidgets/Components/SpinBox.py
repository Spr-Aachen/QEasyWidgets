from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class SpinBoxBase(QSpinBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.Deregistrate(self)

##############################################################################################################################

class DoubleSpinBoxBase(QDoubleSpinBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.Deregistrate(self)

##############################################################################################################################