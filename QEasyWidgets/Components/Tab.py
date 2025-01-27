from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class TabWidgetBase(QTabWidget):
    """
    Base class for tabWidget components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.tabBar().setMinimumSize(84, 42)
        setFont(self, 21)

        StyleSheetBase.Tab.apply(self)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Tab.deregistrate(self)

##############################################################################################################################