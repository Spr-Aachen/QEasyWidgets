from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class SpinBoxBase(QSpinBox):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.Deregistrate(self)


class EmbeddedSpinBox(SpinBoxBase):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

##############################################################################################################################

class DoubleSpinBoxBase(QDoubleSpinBox):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.Deregistrate(self)


class EmbeddedDoubleSpinBox(DoubleSpinBoxBase):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

##############################################################################################################################