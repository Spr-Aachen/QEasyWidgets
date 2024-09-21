from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Button import ButtonBase

##############################################################################################################################

class LineEdit(QLineEdit):
    '''
    '''
    focusedIn = Signal()
    focusedOut = Signal()

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.setAcceptDrops(True)

    def focusInEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedIn.emit()

    def focusOutEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedOut.emit()

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


class LineEditBase(QFrame):
    '''
    '''
    _button = None

    textChanged = Signal(str)
    cursorPositionChanged = Signal(int, int)

    focusedIn = Signal()
    focusedOut = Signal()

    interacted = Signal()

    rectChanged = Signal(QRect)

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        self.LineEdit = LineEdit()
        self.LineEdit.installEventFilter(self)
        self.LineEdit.textChanged.connect(self.textChanged.emit)
        self.LineEdit.textChanged.connect(lambda: self.interacted.emit())
        #self.LineEdit.cursorPositionChanged.connect(self.cursorPositionChanged.emit)
        self.LineEdit.focusedIn.connect(self.focusInEvent)
        self.LineEdit.focusedOut.connect(self.focusOutEvent)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(0)
        HBoxLayout.setContentsMargins(0, 0, 0, 0)
        HBoxLayout.addWidget(self.LineEdit)

        self.ToolTip = QToolTip()
        self.IsAlerted = False

        StyleSheetBase.Edit.Apply(self)

    def focusInEvent(self) -> None:
        self.focusedIn.emit()

    def focusOutEvent(self) -> None:
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

    def setAlignment(self, flag: Qt.AlignmentFlag) -> None:
        self.LineEdit.setAlignment(flag)

    def setReadOnly(self, arg__1: bool) -> None:
        self.LineEdit.setReadOnly(arg__1)

    def clear(self) -> None:
        self.LineEdit.clear()

    def text(self) -> str:
        return self.LineEdit.text()

    def setText(self, arg__1: str) -> None:
        return self.LineEdit.setText(arg__1)

    def placeholderText(self) -> str:
        return self.LineEdit.placeholderText()

    def setPlaceholderText(self, arg__1: str) -> None:
        self.LineEdit.setPlaceholderText(arg__1)

    @property
    def Button(self):
        if self._button is None:
            self.Button = ButtonBase()
        return self._button

    @Button.setter
    def Button(self, Button: ButtonBase):
        Button.setBorderless(True)
        Button.setTransparent(True)
        Button.clicked.connect(self.interacted.emit)
        self.layout().addWidget(Button, alignment = Qt.AlignRight)
        self._button = Button

    def SetFileDialog(self, Mode: str, FileType: Optional[str] = None, Directory: Optional[str] = None, ButtonTooltip: str = "Browse"):
        self.Button.setIcon(IconBase.OpenedFolder)
        self.Button.clicked.connect(
            lambda: self.LineEdit.setText(
                Function_GetFileDialog(
                    Mode = Mode,
                    FileType = FileType,
                    Directory = os.path.expanduser('~/Documents' if platform.system() == "Windows" else '~/') if Directory is None else Directory
                )
            )
        )
        self.Button.setToolTip(ButtonTooltip)

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
        StyleSheetBase.Edit.Apply(self)
        self.showToolTip(Content) if Enable else self.hideToolTip()

##############################################################################################################################

class TextEdit(QTextEdit):
    '''
    '''
    keyEnterPressed = Signal()

    keyEnterBlocked = False

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

    def keyPressEvent(self, e: QKeyEvent) -> None:
        if e.key() in (Qt.Key_Enter, Qt.Key_Return) and not (e.modifiers() == Qt.ShiftModifier) and not self.keyEnterBlocked:
            self.keyEnterPressed.emit()
        super().keyPressEvent(e)

    def blockKeyEnter(self, block: bool) -> None:
        self.keyEnterBlocked = block


class TextEditBase(QFrame):
    '''
    '''
    textChanged = Signal(str)

    keyEnterPressed = Signal()

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.TextEdit = TextEdit()
        self.TextEdit.installEventFilter(self)
        self.TextEdit.textChanged.connect(lambda: self.textChanged.emit(self.toPlainText()))
        self.TextEdit.keyEnterPressed.connect(self.keyEnterPressed.emit)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(0)
        HBoxLayout.setContentsMargins(0, 0, 0, 0)
        HBoxLayout.addWidget(self.TextEdit)

        StyleSheetBase.Edit.Apply(self)

    def toPlainText(self) -> str:
        return self.TextEdit.toPlainText()

    def setText(self, text: str) -> None:
        return self.TextEdit.setText(text)

    def setPlaceholderText(self, placeholderText: str) -> None:
        self.TextEdit.setPlaceholderText(placeholderText)

    def clear(self) -> None:
        self.TextEdit.clear()

    def blockKeyEnter(self, block: bool) -> None:
        self.TextEdit.blockKeyEnter(block)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Edit.Deregistrate(self)

##############################################################################################################################