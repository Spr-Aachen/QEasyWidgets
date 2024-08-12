from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class WidgetBase(QWidget):
    '''
    '''
    resized = Signal()

    def __init__(self,
        parent: Optional[QWidget] = None,
    ):
        super().__init__(parent)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.resized.emit()
        '''
        if self.minimumSizeHint() != self.size():
            self.adjustSize()
        '''
        super().resizeEvent(event)

##############################################################################################################################