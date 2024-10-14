from typing import Optional, overload
#from functools import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class GroupBoxBase(QGroupBox):
    '''
    '''
    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        self.setCheckable(True)
        self.toggled.connect(lambda isChecked: self.collapse() if isChecked else self.expand())

        Function_SetFont(self, 15)

        StyleSheetBase.GroupBox.Apply(self)

    @__init__.register
    def _(self, title: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setTitle(title)

    def expand(self):
        Function_SetWidgetSizeAnimation(self, TargetHeight = self.minimumSizeHint().height()).start()

    def collapse(self):
        Function_SetWidgetSizeAnimation(self, TargetHeight = 0).start()

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.GroupBox.Deregistrate(self)

##############################################################################################################################