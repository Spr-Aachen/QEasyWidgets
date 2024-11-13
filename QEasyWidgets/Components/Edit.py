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
    _isClearButtonEnabled = False

    _fileButton = None
    _isFileButtonEnabled = False

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

        self.spacer = QSpacerItem(0, 0, QSizePolicy.Expanding, QSizePolicy.Minimum)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(0)
        HBoxLayout.setContentsMargins(0, 0, 0, 0)
        HBoxLayout.addSpacerItem(self.spacer)

        self.ToolTip = QToolTip() # TODO Change it to a custom tooltip

        self.IsAlerted = False

        StyleSheetBase.Edit.Apply(self)

    @__init__.register
    def _(self, arg__1: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setText(arg__1)

    def showToolTip(self, content: Optional[str] = None) -> None:
        XPos = 0
        YPos = 0 - self.height()
        self.ToolTip.showText(self.mapToGlobal(QPoint(XPos, YPos)), content) if not self.ToolTip.isVisible() and content is not None else None

    def hideToolTip(self) -> None:
        self.ToolTip.hideText() if self.ToolTip.isVisible() else None

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        position = event.position()
        self.cursorPositionChanged.emit(position.x(), position.y())
        self.interacted.emit()
        super().mouseMoveEvent(event)

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
        if watched is self:
            if event.type() in (QEvent.DragEnter, QEvent.Drop):
                mime_data = event.mimeData()
                if mime_data.hasUrls():
                    event.acceptProposedAction()
        return super().eventFilter(watched, event)

    @property
    def clearButton(self):
        if self._clearButton is None:
            self.clearButton = ClearButton(self)
        return self._clearButton

    @clearButton.setter
    def clearButton(self, clearButton: ClearButton):
        clearButton.setBorderless(True)
        clearButton.setTransparent(True)
        clearButton.clicked.connect(self.clear)
        clearButton.clicked.connect(self.interacted.emit)
        self.layout().insertWidget(self.layout().indexOf(self.spacer) + 1, clearButton, alignment = Qt.AlignRight)
        self.setTextMargins(0, 0, clearButton.minimumSizeHint().width() + self.textMargins().right() + 3, 0)
        self._clearButton = clearButton

    def setClearButtonEnabled(self, enable: bool) -> None:
        self._isClearButtonEnabled = enable
        self.clearButton.setVisible(True if enable and self.hasFocus() else False)

    def isClearButtonEnabled(self) -> bool:
        return self._isClearButtonEnabled

    def focusInEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedIn.emit()
        self.clearButton.show() if self.isClearButtonEnabled() else None
        super().focusInEvent(arg__1)

    def focusOutEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedOut.emit()
        #self.clearButton.hide() if self.isClearButtonEnabled() else None
        super().focusOutEvent(arg__1)

    @property
    def fileButton(self):
        if self._fileButton is None:
            self.fileButton = FileButton(self)
        return self._fileButton

    @fileButton.setter
    def fileButton(self, fileButton: FileButton):
        fileButton.setBorderless(True)
        fileButton.setTransparent(True)
        fileButton.clicked.connect(self.interacted.emit)
        self.layout().insertWidget(self.layout().count(), fileButton, alignment = Qt.AlignRight)
        self.setTextMargins(0, 0, fileButton.minimumSizeHint().width() + self.textMargins().right() + 3, 0)
        self._fileButton = fileButton

    def setFileButtonEnabled(self, enable: bool) -> None:
        self._isFileButtonEnabled = enable
        self.fileButton.setVisible(True if enable else False)

    def isFileButtonEnabled(self) -> bool:
        return self._isFileButtonEnabled

    def setFileDialog(self, mode: str, fileType: Optional[str] = None, directory: Optional[str] = None, buttonTooltip: str = "Browse"):
        self.fileButton.setFileDialog(self, mode, fileType, directory, buttonTooltip)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Edit.Deregistrate(self)

    def setStyleSheet(self, styleSheet: str) -> None:
        AlertStyle = 'LineEditBase {border-color: red;}' if self.IsAlerted else ''
        super().setStyleSheet(styleSheet + AlertStyle)

    def alert(self, enable: bool, content: Optional[str] = None) -> None:
        self.IsAlerted = enable
        #StyleSheetBase.Edit.Apply(self)
        self.showToolTip(content) if enable else self.hideToolTip()

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

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Edit.Deregistrate(self)

##############################################################################################################################