from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Theme import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Widget import WidgetBase
from .Button import RotateButton

##############################################################################################################################

class Folder(QLabel):
    '''
    '''
    _height = 30
    _margin = 6

    isEntered = False

    hoverColor = ThemeColor.Default.color()
    hoverColor.setAlpha(12)

    clicked = Signal()

    def __init__(self,
        parent: Optional[QWidget] = None,
    ):
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
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None,
    ):
        super().__init__(parent)

        self.IsExpanded = True

        self.folder = Folder(self)
        self.folder.clicked.connect(lambda: self.collapse() if self.IsExpanded else self.expand())

        widgetlayout = QGridLayout()
        widgetlayout.setContentsMargins(0, 0, 0, 0)
        widgetlayout.setSpacing(0)
        self.widget = WidgetBase()
        self.widget.setAttribute(Qt.WA_StyledBackground)
        self.widget.setLayout(widgetlayout)
        self.widget.resized.connect(self._resizeHeight)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        layout.addWidget(self.folder)
        layout.addWidget(self.widget)

    def _resizeHeight(self, addedWidget: Optional[QWidget] = None):
        ButtonHeight = self.folder.height()
        LayoutSpacing = self.layout().spacing()
        WidgetLayoutMargins = self.widget.layout().contentsMargins().top() + self.widget.layout().contentsMargins().bottom()
        WidgetHeight = (WidgetLayoutMargins + addedWidget.height()) if addedWidget is not None else self.widget.height()
        AdjustedHeight = ButtonHeight + LayoutSpacing + WidgetHeight if WidgetHeight >=0 else 0
        self.setFixedHeight(AdjustedHeight)

    def addWidget(self, widget: QWidget, title: str):
        self.widget.layout().addWidget(widget)
        self.folder.setText(title)
        def resizeWidgetHeight():
            AdjustedHeight = widget.height()
            self.widget.setFixedHeight(AdjustedHeight) if self.IsExpanded else None
        widget.resized.connect(resizeWidgetHeight) if hasattr(widget, 'resized') else None

    def expand(self):
        setWidgetSizeAnimation(self.widget, targetHeight = self.widget.minimumSizeHint().height()).start()
        self.IsExpanded = True

    def collapse(self):
        setWidgetSizeAnimation(self.widget, targetHeight = 0).start()
        self.IsExpanded = False

    def setText(self, text: str):
        self.folder.setText(text)


class ToolBoxBase(QFrame):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.Pages = []

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(12)

        StyleSheetBase.ToolBox.Apply(self)

    def addItem(self, widget: Union[QWidget, ToolPage], text: str):
        if isinstance(widget, ToolPage):
            page = widget
            page.setText(text)
        else:
            page = ToolPage(self)
            page.addWidget(widget, text)
        self.layout().addWidget(page)
        self.Pages.append(page)
        def resizeHeight():
            AdjustedHeight = page.height()
            self.setFixedHeight(AdjustedHeight)
        page.resized.connect(resizeHeight)

    def widget(self, index: int) -> ToolPage:
        return self.Pages[index]

    def setItemText(self, index: int, text: str) -> None:
        self.widget(index).setText(text)

    def setCurrentIndex(self, index: int) -> None:
        ''''''

    def CurrentIndex(self) -> int:
        ''''''

    def indexOf(self, widget: QWidget) -> int:
        return self.Pages.index(widget if isinstance(widget, ToolPage) else findParentUI(widget, ToolPage))

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ToolBox.Deregistrate(self)

##############################################################################################################################