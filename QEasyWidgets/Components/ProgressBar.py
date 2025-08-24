from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Button import ButtonBase

##############################################################################################################################

class ProgressBarBase(QProgressBar):
    """
    Base class for progressBar components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        StyleSheetBase.ProgressBar.apply(self)

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

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ProgressBar.deregistrate(self)

##############################################################################################################################