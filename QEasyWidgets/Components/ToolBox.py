from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Widget import WidgetBase

##############################################################################################################################

class ToolPage(WidgetBase):
    '''
    '''
    FolderButtonHeight = 30

    FontSize = int(FolderButtonHeight*0.45)
    IndicatorSize = QSize(int(FolderButtonHeight*0.6), int(FolderButtonHeight*0.6))

    def __init__(self,
        parent: Optional[QWidget] = None,
    ):
        super().__init__(parent)

        self.IsExpanded = True

        self.Indicator = QLabel()
        self.Indicator.setFixedSize(self.IndicatorSize)
        layout = QHBoxLayout()
        layout.setContentsMargins(3, 3, 6, 3)
        layout.setSpacing(0)
        layout.addStretch(1)
        layout.addWidget(self.Indicator)
        self.FolderButton = QPushButton()
        self.FolderButton.setFixedHeight(self.FolderButtonHeight)
        self.FolderButton.setLayout(layout)
        self.FolderButton.clicked.connect(lambda: self.collapse() if self.IsExpanded else self.expand())
        Function_SetFont(self.FolderButton, self.FontSize)

        widgetlayout = QGridLayout()
        widgetlayout.setContentsMargins(0, 0, 0, 0)
        widgetlayout.setSpacing(0)
        self.Widget = WidgetBase()
        self.Widget.setAttribute(Qt.WA_StyledBackground)
        self.Widget.setLayout(widgetlayout)
        self.Widget.resized.connect(self._resizeHeight)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        layout.addWidget(self.FolderButton)
        layout.addWidget(self.Widget)

    def _resizeHeight(self, addedWidget: Optional[QWidget] = None):
        ButtonHeight = self.FolderButtonHeight
        LayoutSpacing = self.layout().spacing()
        WidgetLayoutMargins = self.Widget.layout().contentsMargins().top() + self.Widget.layout().contentsMargins().bottom()
        WidgetHeight = (WidgetLayoutMargins + addedWidget.height()) if addedWidget is not None else self.Widget.height()
        AdjustedHeight = ButtonHeight + LayoutSpacing + WidgetHeight if WidgetHeight >=0 else 0
        self.setFixedHeight(AdjustedHeight)

    def addWidget(self, widget: QWidget, title: str):
        self.Widget.layout().addWidget(widget)
        self.FolderButton.setText(title)
        def resizeWidgetHeight():
            AdjustedHeight = widget.height()
            self.Widget.setFixedHeight(AdjustedHeight) if self.IsExpanded else None
        widget.resized.connect(resizeWidgetHeight) if hasattr(widget, 'resized') else None

    def expand(self):
        Function_SetWidgetSizeAnimation(self.Widget, TargetHeight = self.Widget.minimumSizeHint().height()).start()
        self.Indicator.setPixmap(QPixmap(":/ToolBox_Icon/Icons/DownArrow.png").scaled(self.IndicatorSize, Qt.KeepAspectRatio, Qt.SmoothTransformation))
        self.IsExpanded = True

    def collapse(self):
        Function_SetWidgetSizeAnimation(self.Widget, TargetHeight = 0).start()
        self.Indicator.setPixmap(QPixmap(":/ToolBox_Icon/Icons/LeftArrow.png").scaled(self.IndicatorSize, Qt.KeepAspectRatio, Qt.SmoothTransformation))
        self.IsExpanded = False

    def setText(self, text: str):
        self.FolderButton.setText(text)


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
        return self.Pages.index(widget if isinstance(widget, ToolPage) else Function_FindParentUI(widget, ToolPage))

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ToolBox.Deregistrate(self)

##############################################################################################################################