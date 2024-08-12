from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class TextBrowserBase(QTextBrowser):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        StyleSheetBase.Browser.Apply(self)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Browser.Deregistrate(self)

##############################################################################################################################