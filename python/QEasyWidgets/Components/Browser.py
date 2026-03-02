from pathlib import Path
from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from .Widget import SizableWidget
from .ScrollArea import ScrollDelegate
from .Menu import MenuBase

##############################################################################################################################

class TextBrowserBase(QTextBrowser, SizableWidget):
    """
    Base class for textBrowser components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.scrollDelegate = ScrollDelegate(self)

        self.document().setDocumentMargin(0)
        self.document().setDefaultStyleSheet("img { width: 100%; }")

        StyleSheetBase.Browser.apply(self)

    def updateGeometries(self):
        super().updateGeometries()
        if hasattr(self, 'scrollDelegate'):
            self.scrollDelegate._updateViewportMargins()
            # Ensure document width matches viewport to prevent horizontal scrollbars
            self.document().setTextWidth(self.viewport().width())

    def contextMenuEvent(self, e):
        menu = MenuBase(self)
        menu.exec(e.globalPos())

    def loadMarkdown(self, file: Union[str, Path]):
        with open(file, mode = 'r', encoding = 'utf-8') as f:
            md = f.read()
        self.document().setBaseUrl(QUrl.fromLocalFile(str(Path(file).parent) + "/"))
        self.setMarkdown(md)

    def loadHtml(self, file: Union[str, Path]):
        with open(file, mode = 'r', encoding = 'utf-8') as f:
            html = f.read()
        self.document().setBaseUrl(QUrl.fromLocalFile(str(Path(file).parent) + "/"))
        self.setHtml(html)

    def setMaximumLines(self, maxLines: int) -> None:
        self.document().setMaximumBlockCount(maxLines)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Browser.deregistrate(self)

##############################################################################################################################