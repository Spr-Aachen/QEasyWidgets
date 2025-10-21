from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Menu import MenuBase

##############################################################################################################################

class SpinBoxBase(QSpinBox):
    """
    Base class for spinBox components
    """

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.deregistrate(self)

##############################################################################################################################

class DoubleSpinBoxBase(QDoubleSpinBox):
    """
    Base class for doubleSpinBox components
    """

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.deregistrate(self)

##############################################################################################################################