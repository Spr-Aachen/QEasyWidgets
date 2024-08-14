from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class ScrollAreaBase(QScrollArea):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        StyleSheetBase.ScrollArea.Apply(self)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ScrollArea.Deregistrate(self)

##############################################################################################################################