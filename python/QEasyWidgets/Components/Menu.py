from PySide6.QtWidgets import *

from ..Common.StyleSheet import *

##############################################################################################################################

class MenuBase(QMenu):

    def __init__(self, parent = None):
        super().__init__(parent)

        StyleSheetBase.Menu.apply(self)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Menu.deregistrate(self)

##############################################################################################################################