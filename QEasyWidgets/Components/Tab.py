from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class TabWidgetBase(QTabWidget):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.tabBar().setMinimumSize(84, 42)
        Function_SetFont(self, 21)

        StyleSheetBase.Tab.Apply(self)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Tab.Deregistrate(self)

##############################################################################################################################