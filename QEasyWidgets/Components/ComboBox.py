from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class ComboBoxBase(QComboBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.ComboBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ComboBox.Deregistrate(self)

##############################################################################################################################