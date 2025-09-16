from typing import Optional, overload
from PyEasyUtils import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Widget import SizableWidget

##############################################################################################################################

class GroupBoxBase(QGroupBox, SizableWidget):
    """
    Base class for groupBox components
    """
    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        self.setCheckable(True)
        self.toggled.connect(lambda isChecked: self.collapse() if isChecked else self.expand())

        setFont(self, 15)

        StyleSheetBase.GroupBox.apply(self)

    @__init__.register
    def _(self, title: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setTitle(title)

    def expand(self):
        setWidgetSizeAnimation(self, targetHeight = self.minimumSizeHint().height()).start()

    def collapse(self):
        setWidgetSizeAnimation(self, targetHeight = 0).start()

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.GroupBox.deregistrate(self)

##############################################################################################################################