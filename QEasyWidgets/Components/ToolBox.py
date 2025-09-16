from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Theme import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Widget import WidgetBase
from .Frame import FrameBase
from .Button import RotateButton

##############################################################################################################################

class Folder(QLabel):
    """
    """
    _height = 30
    _margin = 6

    isEntered = False

    hoverColor = ThemeColor.Default.color()
    hoverColor.setAlpha(12)

    clicked = Signal()

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.installEventFilter(self)
        self.setFixedHeight(self._height)
        setFont(self, int(self._height*0.45))

        self.folderButton = RotateButton()
        self.folderButton.setFixedSize(self._height - self._margin, self._height - self._margin)
        self.folderButton.clicked.connect(self.clicked.emit)

        layout = QHBoxLayout(self)
        layout.setContentsMargins(self._margin, self._margin, self._margin, self._margin)
        layout.setSpacing(0)
        layout.addWidget(self.folderButton, alignment = Qt.AlignRight)

    def eventFilter(self, watched: QObject, event: QEvent):
        if watched is self:
            if event.type() == QEvent.MouseButtonRelease and event.button() == Qt.LeftButton:
                self.folderButton.click()
        return super().eventFilter(watched, event)

    def enterEvent(self, event):
        self.isEntered = True
        super().enterEvent(event)

    def leaveEvent(self, event):
        self.isEntered = False
        super().leaveEvent(event)

    def printEvent(self, e: QPaintEvent):
        super().printEvent(e)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.fillRect(self.rect(), QColor(self.hoverColor) if self.isEntered else QColor(Qt.transparent))


class ToolPage(WidgetBase):
    """
    Base class for toolPage components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.isExpanded = True

        self.folder = Folder(self)
        self.folder.clicked.connect(lambda: self.collapse() if self.isExpanded else self.expand())

        widgetlayout = QGridLayout()
        widgetlayout.setContentsMargins(0, 0, 0, 0)
        widgetlayout.setSpacing(0)
        self.widget = WidgetBase()
        self.widget.setAttribute(Qt.WA_StyledBackground)
        self.widget.setLayout(widgetlayout)
        self.widget.resized.connect(self.updateHeight)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        layout.addWidget(self.folder)
        layout.addWidget(self.widget)

    def updateHeight(self, addedWidget: Optional[QWidget] = None):
        buttonHeight = self.folder.height()
        layoutSpacing = self.layout().spacing()
        widgetLayoutMargins = self.widget.layout().contentsMargins().top() + self.widget.layout().contentsMargins().bottom()
        widgetHeight = (widgetLayoutMargins + addedWidget.height()) if addedWidget is not None else self.widget.height()
        adjustedHeight = buttonHeight + layoutSpacing + widgetHeight if widgetHeight >=0 else 0
        self.setFixedHeight(adjustedHeight)

    def addWidget(self, widget: QWidget):
        self.widget.layout().addWidget(widget)

    def expand(self):
        setWidgetSizeAnimation(self.widget, targetHeight = self.widget.minimumSizeHint().height()).start()
        self.isExpanded = True

    def collapse(self):
        setWidgetSizeAnimation(self.widget, targetHeight = 0).start()
        self.isExpanded = False

    def setText(self, text: str):
        self.folder.setText(text)

    def text(self):
        return self.folder.text()


class ToolBoxBase(FrameBase):
    """
    Base class for toolBox components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.toolPages: list[ToolPage] = []

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(12)

        StyleSheetBase.ToolBox.apply(self)

    def updateHeight(self):
        layoutSpacing = self.layout().spacing() * (len(self.toolPages) - 1) if len(self.toolPages) > 1 else 0
        layoutMargins = self.layout().contentsMargins().top() + self.layout().contentsMargins().bottom()
        toolPagesHeight = layoutMargins
        for toolPage in self.toolPages:
            toolPagesHeight += toolPage.height()
        adjustedHeight = layoutSpacing + toolPagesHeight
        self.setFixedHeight(adjustedHeight)

    def addItem(self, widget: QWidget, text: str):
        for toolPage in self.toolPages:
            if toolPage.text() == text:
                toolPage.addWidget(widget)
                break
        else:
            toolPage = ToolPage(self)
            toolPage.setText(text)
            toolPage.addWidget(widget)
            toolPage.resized.connect(self.updateHeight)
            self.layout().addWidget(toolPage)
            self.toolPages.append(toolPage)

    def widget(self, index: int) -> ToolPage:
        return self.toolPages[index]

    def setItemText(self, index: int, text: str) -> None:
        self.widget(index).setText(text)

    def setCurrentIndex(self, index: int) -> None:
        ''''''

    def currentIndex(self) -> int:
        ''''''

    def indexOf(self, widget: QWidget) -> int:
        return self.toolPages.index(widget if isinstance(widget, ToolPage) else findParent(widget, ToolPage))

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ToolBox.deregistrate(self)

##############################################################################################################################