from typing import Optional, overload
#from functools import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Button import ClearButton, FileButton

##############################################################################################################################

class LineEditBase(QLineEdit):
    '''
    '''
    _clearButton = None
    _isClearButtonEnabled = True

    _fileButton = None

    cursorPositionChanged = Signal(int, int)

    focusedIn = Signal()
    focusedOut = Signal()

    interacted = Signal()

    rectChanged = Signal(QRect)

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        self.installEventFilter(self)
        self.textChanged.connect(lambda: self.interacted.emit())
        self.textChanged.connect(lambda: self.setClearButtonEnabled(True if len(self.text()) > 0 else False))

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(0)
        HBoxLayout.setContentsMargins(0, 0, 0, 0)
        HBoxLayout.addSpacerItem(QSpacerItem(0, 0, QSizePolicy.Expanding, QSizePolicy.Minimum))

        self.ToolTip = QToolTip() # TODO Change it to a custom tooltip

        self.IsAlerted = False

        StyleSheetBase.Edit.Apply(self)

    @__init__.register
    def _(self, arg__1: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setText(arg__1)

    def focusInEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedIn.emit()

    def focusOutEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedOut.emit()

    def showToolTip(self, Content: Optional[str] = None) -> None:
        XPos = 0
        YPos = 0 - self.height()
        self.ToolTip.showText(self.mapToGlobal(QPoint(XPos, YPos)), Content) if not self.ToolTip.isVisible() and Content is not None else None

    def hideToolTip(self) -> None:
        self.ToolTip.hideText() if self.ToolTip.isVisible() else None

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        position = event.position()
        self.cursorPositionChanged.emit(position.x(), position.y())
        self.interacted.emit()

    def moveEvent(self, event: QMoveEvent) -> None:
        self.rectChanged.emit(self.rect())

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.rectChanged.emit(self.rect())

    def dragEnterEvent(self, event: QDragEnterEvent):
        if event.mimeData().hasUrls():
            event.acceptProposedAction()

    def dropEvent(self, event: QDropEvent):
        if event.mimeData().hasUrls():
            paths = [url.toLocalFile() for url in event.mimeData().urls()]
            self.setText(paths[0] if len(paths) == 1 else ", ".join(paths))

    def eventFilter(self, watched: QObject, event: QEvent) -> bool:
        if event.type() in (QEvent.DragEnter, QEvent.Drop):
            mime_data = event.mimeData()
            if mime_data.hasUrls():
                event.acceptProposedAction()
        return super().eventFilter(watched, event)

    @property
    def clearButton(self):
        if self._clearButton is None:
            self.clearButton = ClearButton()
        return self._clearButton

    @clearButton.setter
    def clearButton(self, clearButton: ClearButton):
        clearButton.setBorderless(True)
        clearButton.clicked.connect(self.clear)
        clearButton.clicked.connect(self.interacted.emit)
        self.layout().addWidget(clearButton, alignment = Qt.AlignRight)
        self._clearButton = clearButton

    def setClearButtonEnabled(self, enable: bool) -> None:
        self._isClearButtonEnabled = enable
        self.clearButton.setVisible(True if enable else False)

    def isClearButtonEnabled(self) -> bool:
        return self._isClearButtonEnabled

    @property
    def fileButton(self):
        if self._fileButton is None:
            self.fileButton = FileButton()
        return self._fileButton

    @fileButton.setter
    def fileButton(self, fileButton: FileButton):
        fileButton.setBorderless(True)
        fileButton.clicked.connect(self.interacted.emit)
        self.layout().addWidget(fileButton, alignment = Qt.AlignRight)
        self._fileButton = fileButton

    def setFileDialog(self, Mode: str, FileType: Optional[str] = None, Directory: Optional[str] = None, ButtonTooltip: str = "Browse"):
        self.fileButton.setFileDialog(Mode, FileType, Directory, ButtonTooltip)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Edit.Deregistrate(self)

    def setStyleSheet(self, styleSheet: str) -> None:
        AlertStyle = 'LineEditBase {border-color: red;}' if self.IsAlerted else ''
        super().setStyleSheet(styleSheet + AlertStyle)

    def Alert(self, Enable: bool, Content: Optional[str] = None) -> None:
        self.IsAlerted = Enable
        #StyleSheetBase.Edit.Apply(self)
        self.showToolTip(Content) if Enable else self.hideToolTip()

##############################################################################################################################

class TextEditBase(QTextEdit):
    '''
    '''
    keyEnterPressed = Signal()

    keyEnterBlocked = False

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        #self.installEventFilter(self)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(0)
        HBoxLayout.setContentsMargins(0, 0, 0, 0)

        StyleSheetBase.Edit.Apply(self)

    @__init__.register
    def _(self, text: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setText(text)

    def keyPressEvent(self, e: QKeyEvent) -> None:
        if e.key() in (Qt.Key_Enter, Qt.Key_Return) and not (e.modifiers() == Qt.ShiftModifier) and not self.keyEnterBlocked:
            self.keyEnterPressed.emit()
        super().keyPressEvent(e)

    def blockKeyEnter(self, block: bool) -> None:
        self.keyEnterBlocked = block

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Edit.Deregistrate(self)

##############################################################################################################################