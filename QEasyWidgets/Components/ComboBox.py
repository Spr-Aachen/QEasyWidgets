from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class ComboBoxBase(QComboBox):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.ComboBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ComboBox.Deregistrate(self)

##############################################################################################################################