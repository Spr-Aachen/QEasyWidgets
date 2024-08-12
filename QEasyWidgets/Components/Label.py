from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class LabelBase(QLabel):
    '''
    '''
    resized = Signal()

    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        StyleSheetBase.Label.Apply(self)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.resized.emit()
        super().resizeEvent(event)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Label.Deregistrate(self)

##############################################################################################################################