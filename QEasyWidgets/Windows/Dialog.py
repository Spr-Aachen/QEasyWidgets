from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap, QIcon, QMouseEvent
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from ..Resources.Sources import *
from .Window import WindowBase

##############################################################################################################################

class DialogBase(WindowBase, QDialog):
    '''
    '''
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

        StyleSheetBase.Dialog.Apply(self)

    def exec(self) -> int:
        Result = super().exec()
        self.closed.emit()
        return Result

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        return

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Dialog.Deregistrate(self)

##############################################################################################################################

class MessageBoxBase(DialogBase):
    '''
    '''
    StandardButtonDict = {
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
        QMessageBox.Ok | QMessageBox.Cancel:             QDialogButtonBox.Ok | QDialogButtonBox.Cancel,
        QMessageBox.Yes | QMessageBox.No:                QDialogButtonBox.Yes | QDialogButtonBox.No,
        QMessageBox.Retry | QMessageBox.Ignore:          QDialogButtonBox.Retry | QDialogButtonBox.Ignore,
        QMessageBox.Open | QMessageBox.Close:            QDialogButtonBox.Open | QDialogButtonBox.Close,
        QMessageBox.Save | QMessageBox.Discard:          QDialogButtonBox.Save | QDialogButtonBox.Discard,
        QMessageBox.Apply | QMessageBox.RestoreDefaults: QDialogButtonBox.Apply | QDialogButtonBox.RestoreDefaults
    }

    StandardIconDict = {
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

        self.PicLabel = QLabel()
        self.TextLabel = QLabel()
        PicTextLayout = QHBoxLayout()
        PicTextLayout.setContentsMargins(0, 0, 0, 0)
        PicTextLayout.setSpacing(0)
        PicTextLayout.addWidget(self.PicLabel, stretch = 0)
        PicTextLayout.addWidget(self.TextLabel, stretch = 1)

        self.ButtonBox = QDialogButtonBox()
        self.ButtonBox.clicked.connect(self.updateClickedButton)
        self.ButtonBox.accepted.connect(self.accept)
        self.ButtonBox.rejected.connect(self.reject)
        self.ButtonBox.setOrientation(Qt.Horizontal)
        self.ButtonBox.setStyleSheet("padding: 6px 18px 6px 18px;")

        self.Layout = QVBoxLayout(self)
        self.Layout.setContentsMargins(21, 12, 21, 12)
        self.Layout.setSpacing(12)
        self.Layout.addLayout(PicTextLayout)
        self.Layout.addWidget(self.ButtonBox)

    def updateClickedButton(self, button: QAbstractButton):
        self.ClickedButton = FindKey(self.StandardButtonDict, self.ButtonBox.standardButton(button))

    def exec(self) -> int:
        Result = super().exec()
        return self.ClickedButton if hasattr(self, 'ClickedButton') else Result

    def setStandardButtons(self, buttons: QMessageBox.StandardButton) -> None:
        buttons = self.StandardButtonDict.get(buttons, buttons)
        if isinstance(buttons, QMessageBox.StandardButton):
            pass
        self.ButtonBox.setStandardButtons(buttons)

    def setWindowIcon(self, icon: Union[QIcon, QPixmap, QStyle.StandardPixmap]) -> None:
        icon = self.StandardIconDict.get(icon, icon)
        if isinstance(icon, QStyle.StandardPixmap):
            icon = QApplication.style().standardIcon(icon)
        if isinstance(icon, (QIcon, QPixmap)):
            pass
        super().setWindowIcon(icon)

    def setIcon(self, icon: Union[QIcon, QPixmap, QStyle.StandardPixmap]) -> None:
        icon = self.StandardIconDict.get(icon, icon)
        Length = int(min(self.width(), self.height()) / 6)
        if isinstance(icon, QStyle.StandardPixmap):
            standardIcon = QApplication.style().standardIcon(icon)
            icon = standardIcon.pixmap(standardIcon.actualSize(QSize(Length, Length)))
        if isinstance(icon, QIcon):
            icon = icon.pixmap(icon.actualSize(QSize(Length, Length)))
        if isinstance(icon, QPixmap):
            pass
        self.PicLabel.setPixmap(icon)

    def setText(self, text: str, textsize: float = 11.1, textweight: int = 420):
        Function_SetText(
            Widget = self.TextLabel,
            Text = SetRichText(
                Title = text,
                TitleSize = textsize,
                TitleWeight = textweight,
                TitleAlign = 'center'
            )
        )

##############################################################################################################################