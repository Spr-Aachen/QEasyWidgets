from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class GroupBoxBase(QGroupBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setCheckable(True)
        self.toggled.connect(lambda isChecked: self.collapse() if isChecked else self.expand())

        StyleSheetBase.GroupBox.Apply(self)

    def expand(self):
        Function_SetWidgetSizeAnimation(self, TargetHeight = self.minimumSizeHint().height()).start()

    def collapse(self):
        Function_SetWidgetSizeAnimation(self, TargetHeight = 0).start()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.GroupBox.Deregistrate(self)

##############################################################################################################################