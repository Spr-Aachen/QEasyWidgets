from pathlib import Path
from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from ..Resources.Sources import *
from .Menu import MenuBase

##############################################################################################################################

class TextBrowserBase(QTextBrowser):
    """
    Base class for textBrowser components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        StyleSheetBase.Browser.apply(self)

    def getCurrentWidth(self):
        return getWidth(self)

    def setCurrentWidth(self, w: int):
        self.setFixedWidth(w)

    currentWidth = Property(int, getCurrentWidth, setCurrentWidth)

    def getCurrentHeight(self):
        return getHeight(self)

    def setCurrentHeight(self, w: int):
        self.setFixedHeight(w)

    currentHeight = Property(int, getCurrentHeight, setCurrentHeight)

    def loadMarkdown(self, file: Union[str, Path]):
        with open(file, mode = 'r', encoding = 'utf-8') as f:
            md = f.read()
        self.setMarkdown(md)

    def loadHtml(self, file: Union[str, Path]):
        with open(file, mode = 'r', encoding = 'utf-8') as f:
            html = f.read()
        self.setHtml(html)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Browser.deregistrate(self)

##############################################################################################################################