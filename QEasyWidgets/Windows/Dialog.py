from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap, QIcon, QMouseEvent
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from .Window import WindowBase
from ..Components.Label import LabelBase
from ..Components.Browser import TextBrowserBase
from ..Components.Edit import LineEditBase

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
        QDialog.__init__(self, None, f) #QDialog.__init__(self, parent, f)
        WindowBase.__init__(self, min_width, min_height)

        self.setFrameless(SetStrechable = False)

        self.TitleBar.MinimizeButton.hide()
        self.TitleBar.MinimizeButton.deleteLater()
        self.TitleBar.MaximizeButton.hide()
        self.TitleBar.MaximizeButton.deleteLater()

        self.showed.connect(lambda: parent.ShowMask(True)) if isinstance(parent, WindowBase) else None
        self.closed.connect(lambda: parent.ShowMask(False)) if isinstance(parent, WindowBase) else None

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

    standardButtonDict = {
        QMessageBox.NoButton:        QDialogButtonBox.NoButton,
        QMessageBox.Ok:              QDialogButtonBox.Ok,
        QMessageBox.Cancel:          QDialogButtonBox.Cancel,
        QMessageBox.Yes:             QDialogButtonBox.Yes,
        QMessageBox.No:              QDialogButtonBox.No,
        QMessageBox.Retry:           QDialogButtonBox.Retry,
        QMessageBox.Ignore:          QDialogButtonBox.Ignore,
        QMessageBox.Open:            QDialogButtonBox.Open,
        QMessageBox.Close:           QDialogButtonBox.Close,
        QMessageBox.Save:            QDialogButtonBox.Save,
        QMessageBox.Discard:         QDialogButtonBox.Discard,
        QMessageBox.Apply:           QDialogButtonBox.Apply,
        QMessageBox.RestoreDefaults: QDialogButtonBox.RestoreDefaults,
    }

    standardIconDict = {
        QMessageBox.Question:    QStyle.SP_MessageBoxQuestion,
        QMessageBox.Information: QStyle.SP_MessageBoxInformation,
        QMessageBox.Warning:     QStyle.SP_MessageBoxWarning,
        QMessageBox.Critical:    QStyle.SP_MessageBoxCritical
    }

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

        self.buttonBox = QDialogButtonBox()
        self.buttonBox.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        self.buttonBox.clicked.connect(self.updateClickedButton)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
        self.buttonBox.setOrientation(Qt.Horizontal)
        self.buttonBox.setStyleSheet("padding: 6px 18px 6px 18px;")

        layout = QVBoxLayout(self)
        layout.setContentsMargins(21, 12, 21, 12)
        layout.setSpacing(12)
        layout.addLayout(labelLayout)
        layout.addWidget(self.buttonBox)

    def updateClickedButton(self, button: QAbstractButton):
        self.clickedButton = QMessageBox.NoButton
        for messageBoxButton, dialogBoxButton in self.standardButtonDict.items():
            if self.buttonBox.standardButton(button) & dialogBoxButton:
                self.clickedButton |= messageBoxButton

    def exec(self) -> int:
        result = super().exec()
        return self.clickedButton if hasattr(self, 'clickedButton') else result

    def setStandardButtons(self, buttons: QMessageBox.StandardButton) -> None:
        standardButtons = QDialogButtonBox.NoButton
        for messageBoxButton, dialogBoxButton in self.standardButtonDict.items():
            if buttons & messageBoxButton:
                standardButtons |= dialogBoxButton
        self.buttonBox.setStandardButtons(standardButtons)

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
        Function_SetText(
            Widget = self.textLabel,
            Text = SetRichText(
                Title = text,
                TitleSize = textsize,
                TitleWeight = textweight,
                TitleAlign = 'center'
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
        self.layout().insertWidget(self.layout().indexOf(self.buttonBox), detailedTextBrowser, stretch = 1)
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