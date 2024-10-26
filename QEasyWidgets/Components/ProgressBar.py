from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Button import ButtonBase

##############################################################################################################################

class ProgressBarBase(QProgressBar):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        StyleSheetBase.ProgressBar.Apply(self)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ProgressBar.Deregistrate(self)

##############################################################################################################################