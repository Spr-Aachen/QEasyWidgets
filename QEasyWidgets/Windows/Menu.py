from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class MenuBase(QMenu):

    def __init__(self, parent = None):
        super().__init__(parent)

        StyleSheetBase.Menu.Apply(self)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Menu.Deregistrate(self)

##############################################################################################################################