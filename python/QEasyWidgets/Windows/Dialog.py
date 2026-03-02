from PyEasyUtils import setRichText
from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap, QIcon, QMouseEvent
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from .Window import WindowBase
from ..Components.Label import LabelBase
from ..Components.Browser import TextBrowserBase
from ..Components.Edit import LineEditBase
from ..Components.Button import ButtonBase

##############################################################################################################################

class DialogBase(WindowBase, QDialog):
    """
    """
    def __init__(self,
        parent: Optional[QWidget] = None,
        f: Qt.WindowType = Qt.Dialog,
        min_width: int = 630,
        min_height: int = 420
    ):
        QDialog.__init__(self, parent, f) #QDialog.__init__(self, None, f)
        WindowBase.__init__(self, min_width, min_height)

        self.setFrameless(setStrechable = False)

        self.titleBar.minimizeButton.hide()
        self.titleBar.minimizeButton.deleteLater()
        self.titleBar.maximizeButton.hide()
        self.titleBar.maximizeButton.deleteLater()

        parentWindow = findParent(self, WindowBase) or (parent.window() if parent else None)
        self.showed.connect(lambda: parentWindow.showMask(True)) if isinstance(parentWindow, WindowBase) else None
        self.closed.connect(lambda: parentWindow.showMask(False)) if isinstance(parentWindow, WindowBase) else None

    def exec(self) -> int:
        result = super().exec()
        self.closed.emit()
        return result

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        return

##############################################################################################################################

class MessageBoxBase(DialogBase):
    """
    """
    _detailedTextBrowser = None

    standardIconDict = {
        QMessageBox.Question:    QStyle.SP_MessageBoxQuestion,
        QMessageBox.Information: QStyle.SP_MessageBoxInformation,
        QMessageBox.Warning:     QStyle.SP_MessageBoxWarning,
        QMessageBox.Critical:    QStyle.SP_MessageBoxCritical
    }

    _buttonTextMap = {
        QMessageBox.Ok:              "OK",
        QMessageBox.Cancel:          "Cancel",
        QMessageBox.Yes:             "Yes",
        QMessageBox.No:              "No",
        QMessageBox.Retry:           "Retry",
        QMessageBox.Ignore:          "Ignore",
        QMessageBox.Open:            "Open",
        QMessageBox.Close:           "Close",
        QMessageBox.Save:            "Save",
        QMessageBox.Discard:         "Discard",
        QMessageBox.Apply:           "Apply",
        QMessageBox.RestoreDefaults: "Restore Defaults",
        QMessageBox.Help:            "Help",
        QMessageBox.Abort:           "Abort",
    }

    _acceptRoles = [QMessageBox.Ok, QMessageBox.Yes, QMessageBox.Save, QMessageBox.Open, QMessageBox.Retry, QMessageBox.Apply]

    def __init__(self,
        parent: Optional[QWidget] = None,
        min_width = 360,
        min_height = 210
    ):  
        super().__init__(parent, Qt.Dialog, min_width, min_height)

        self.iconLabel = QLabel()
        self.iconLabel.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        self.textLabel = LabelBase()
        self.textLabel.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        labelLayout = QHBoxLayout()
        labelLayout.setContentsMargins(0, 0, 0, 0)
        labelLayout.setSpacing(12)
        labelLayout.addWidget(self.iconLabel, stretch = 0)
        labelLayout.addWidget(self.textLabel, stretch = 1)

        self.buttonLayout = QHBoxLayout()
        self.buttonLayout.setContentsMargins(0, 0, 0, 0)
        self.buttonLayout.setSpacing(12)
        self.buttonLayout.setAlignment(Qt.AlignRight)

        self.buttonGroup = QFrame()
        self.buttonGroup.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        self.buttonGroup.setLayout(self.buttonLayout)
        self.buttonGroup.setStyleSheet("padding: 6px 18px 6px 18px;")

        layout = QVBoxLayout(self)
        layout.setContentsMargins(21, 12, 21, 12)
        layout.setSpacing(12)
        layout.addLayout(labelLayout)
        layout.addWidget(self.buttonGroup)
        
        self.clickedButton = QMessageBox.NoButton

    def exec(self) -> int:
        result = super().exec()
        return self.clickedButton if hasattr(self, 'clickedButton') else result

    def _onButtonClicked(self, flag):
        self.clickedButton = flag
        if flag in self._acceptRoles:
            self.accept()
        else:
            self.reject()

    def _addButton(self, flag, is_default=False):
        text = self._buttonTextMap.get(flag, "Button")
        btn = ButtonBase(text)
        btn.clicked.connect(lambda: self._onButtonClicked(flag))
        self.buttonLayout.addWidget(btn)
        if is_default:
            btn.setFocus()

    def setStandardButtons(self, buttons: QMessageBox.StandardButton) -> None:
        while self.buttonLayout.count():
            item = self.buttonLayout.takeAt(0)
            widget = item.widget()
            if widget:
                widget.deleteLater()
        
        ordered_flags = [
            QMessageBox.Help,
            QMessageBox.RestoreDefaults,
            QMessageBox.Open,
            QMessageBox.Save,
            QMessageBox.Apply,
            QMessageBox.Yes,
            QMessageBox.Ok,
            QMessageBox.Retry,
            QMessageBox.No,
            QMessageBox.Discard,
            QMessageBox.Cancel,
            QMessageBox.Close,
            QMessageBox.Abort,
            QMessageBox.Ignore,
        ]
        
        has_default = False
        for flag in ordered_flags:
            if buttons & flag:
                self._addButton(flag, not has_default)
                has_default = True

    def setWindowIcon(self, icon: Union[QIcon, QPixmap, QStyle.StandardPixmap]) -> None:
        icon = self.standardIconDict.get(icon, icon)
        if isinstance(icon, QStyle.StandardPixmap):
            icon = QApplication.style().standardIcon(icon)
        if isinstance(icon, (QIcon, QPixmap)):
            pass
        super().setWindowIcon(icon)

    def setIcon(self, icon: Union[QIcon, QPixmap, QStyle.StandardPixmap]) -> None:
        icon = self.standardIconDict.get(icon, icon)
        Length = int(min(self.width(), self.height()) / 6)
        if isinstance(icon, QStyle.StandardPixmap):
            standardIcon = QApplication.style().standardIcon(icon)
            icon = standardIcon.pixmap(standardIcon.actualSize(QSize(Length, Length)))
        if isinstance(icon, QIcon):
            icon = icon.pixmap(icon.actualSize(QSize(Length, Length)))
        if isinstance(icon, QPixmap):
            pass
        self.iconLabel.setPixmap(icon)

    def setText(self, text: str, textsize: float = 11.1, textweight: int = 420):
        setText(
            widget = self.textLabel,
            text = setRichText(
                text = text,
                align = 'center',
                size = textsize,
                weight = textweight,
            )
        )

    @property
    def detailedTextBrowser(self):
        if self._detailedTextBrowser is None:
            self.detailedTextBrowser = TextBrowserBase()
        return self._detailedTextBrowser

    @detailedTextBrowser.setter
    def detailedTextBrowser(self, detailedTextBrowser: TextBrowserBase):
        detailedTextBrowser.setBorderless(False)
        self.layout().insertWidget(self.layout().indexOf(self.buttonGroup), detailedTextBrowser, stretch = 1)
        self._detailedTextBrowser = detailedTextBrowser

    def setDetailedText(self, text: str):
        self.detailedTextBrowser.setMarkdown(text)

    def getText(self, title: str, label: str, echo: QLineEdit.EchoMode = None, text: str = None, flags: Qt.WindowType = Qt.Dialog, inputMethodHints: Qt.InputMethodHint = None):
        self.setWindowFlags(flags)
        self.setText(title) #self.setWindowTitle(title)
        inputLabel = LabelBase()
        inputLabel.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        inputArea = LineEditBase()
        inputArea.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        inputLayout = QHBoxLayout()
        inputLayout.setContentsMargins(0, 0, 0, 0)
        inputLayout.setSpacing(12)
        inputLayout.addWidget(inputLabel, stretch = 0)
        inputLayout.addWidget(inputArea, stretch = 1)
        inputLabel.setText(label)
        inputArea.setEchoMode(echo) if echo else None
        inputArea.setText(text) if text else None
        inputArea.setInputMethodHints(inputMethodHints) if inputMethodHints else None
        self.layout().insertLayout(1, inputLayout, stretch = 1)
        self.setStandardButtons(QMessageBox.Ok | QMessageBox.Cancel)
        result = self.exec()
        return inputArea.text(), True if result == QMessageBox.Ok else False

    @staticmethod
    def pop(
        windowToMask: Optional[WindowBase] = None,
        messageType: object = QMessageBox.Information,
        windowTitle: str = ...,
        text: str = ...,
        detailedText: Optional[str] = None,
        buttons: object = QMessageBox.Ok,
        buttonEvents: dict = {}
    ):
        """
        Function to pop up a msgbox
        """
        msgBox = MessageBoxBase(windowToMask)

        msgBox.setIcon(messageType)
        msgBox.setWindowTitle(windowTitle)
        msgBox.setText(text)
        msgBox.setDetailedText(detailedText) if detailedText else None
        msgBox.setStandardButtons(buttons)

        result = msgBox.exec()

        buttonEvents[result]() if result in list(buttonEvents.keys()) else None

        return result

##############################################################################################################################

class InputDialogBase(MessageBoxBase):
    """
    """
    def __init__(self,
        parent: Optional[QWidget] = None,
        min_width = 360,
        min_height = 210
    ):  
        super().__init__(parent, min_width, min_height)

    @staticmethod
    def getText(parent: QWidget, title: str, label: str, echo: QLineEdit.EchoMode = None, text: str = None, flags: Qt.WindowType = Qt.Dialog, inputMethodHints: Qt.InputMethodHint = None):
        msgBox = MessageBoxBase(parent, 420, 210)
        return msgBox.getText(title, label, echo, text, flags, inputMethodHints)

##############################################################################################################################