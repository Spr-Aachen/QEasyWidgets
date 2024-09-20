from typing import Optional
from PySide6.QtCore import Qt, QEvent, QEventLoop
from PySide6.QtGui import QCloseEvent
from PySide6.QtWidgets import QWidget, QMainWindow

from ..Common.QFunctions import *
from .FramelessWindow import WindowBase

##############################################################################################################################

class MainWindowBase(WindowBase, QMainWindow):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None,
        flags: Qt.WindowType = Qt.Window | Qt.WindowSystemMenuHint | Qt.WindowMinMaxButtonsHint,
        min_width: int = 1280,
        min_height: int = 720
    ):
        QMainWindow.__init__(self, parent, flags)
        WindowBase.__init__(self, min_width, min_height)

        self.setFrameless()

        self.CentralLayout = QGridLayout()
        self.CentralWidget = QWidget(self)
        self.CentralWidget.setLayout(self.CentralLayout)
        self.setCentralWidget(self.CentralWidget)

    def setCentralWidget(self, CentralWidget: Optional[QWidget]) -> None:
        try:
            super().takeCentralWidget(self.CentralWidget)
            self.CentralWidget.deleteLater()
            self.CentralWidget.hide()
        except:
            pass
        if CentralWidget is not None:
            self.CentralWidget = CentralWidget
            super().setCentralWidget(self.CentralWidget)
            self.CentralWidget.setParent(self) if self.CentralWidget.parent() is None else None
            self.CentralWidget.raise_() if self.CentralWidget.isHidden() else None
        else:
            self.CentralWidget = None


class ChildWindowBase(WindowBase, QWidget):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None,
        f: Qt.WindowType = Qt.Widget,
        min_width: int = 630,
        min_height: int = 420
    ):
        QWidget.__init__(self, None, f) #QWidget.__init__(self, parent, f)
        WindowBase.__init__(self, min_width, min_height)

        self.setFrameless()

        self.setWindowModality(Qt.ApplicationModal)

        self.EventLoop = QEventLoop(self)

        self.showed.connect(lambda: parent.ShowMask(True)) if isinstance(parent, WindowBase) else None
        self.closed.connect(lambda: parent.ShowMask(False)) if isinstance(parent, WindowBase) else None

    def exec(self) -> int:
        self.show()
        Result = self.EventLoop.exec()
        self.closed.emit()
        return Result

    def closeEvent(self, event: QCloseEvent) -> None:
        Result = self.EventLoop.exit()
        super().closeEvent(event)

##############################################################################################################################