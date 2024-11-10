from typing import Optional
from PySide6.QtCore import Qt, Signal, QPoint, QRect, QEvent
from PySide6.QtGui import QFont, QCursor, QMouseEvent, QShowEvent, QCloseEvent, QMoveEvent, QResizeEvent
from PySide6.QtWidgets import QApplication, QWidget, QLabel

from ...Common.Theme import BackgroundColorAnimationBase
from ...Common.StyleSheet import StyleSheetBase
from ...Common.QFunctions import *
from .Bar import TitleBarBase

##############################################################################################################################

class WindowBase(BackgroundColorAnimationBase):
    '''
    '''
    showed = Signal()
    closed = Signal()

    langChanged = Signal()

    rectChanged = Signal(QRect)

    edge_size = 3 # 窗体边缘尺寸（出现缩放标记的范围）

    def __init__(self,
        min_width = 630, # 窗体的最小宽度
        min_height = 420, # 窗体的最小高度
        *args, **kwargs
    ):
        super().__init__(*args, **kwargs)

        self.TitleBar = TitleBarBase(self)

        self.Mask = QLabel(self)
        self.rectChanged.connect(self.Mask.setGeometry)
        self.Mask.setStyleSheet('background-color: rgba(0, 0, 0, 111);')
        self.Mask.setAlignment(Qt.AlignCenter)
        self.Mask.setFont(QFont('Microsoft YaHei', int(min_height / 10), QFont.Bold))
        self.Mask.hide()

        self.resize(min_width, min_height)

    def _check_ifdraggable(self, pos) -> bool:
        return (0 < pos.x() < self.width() and 0 < pos.y() < self.TitleBar.height()) if self.TitleBar is not None else False

    def _move_window(self, pos) -> None:
        self.windowHandle().startSystemMove()
        QApplication.instance().postEvent(
            self.windowHandle(),
            QMouseEvent(
                QEvent.MouseButtonRelease,
                QPoint(-1, -1),
                Qt.LeftButton,
                Qt.NoButton,
                Qt.NoModifier
            )
        )

    def _resize_window(self, pos, edges) -> None:
        self.windowHandle().startSystemResize(edges) if edges is not None else None

    def event(self, event: QEvent) -> bool:
        self.langChanged.emit() if event.type() == QEvent.LanguageChange else None
        return super().event(event)

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
            return

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if self._check_ifdraggable(event.position()) == True and event.buttons() == Qt.MouseButton.LeftButton:
            self.setCursor(Qt.CursorShape.OpenHandCursor)
            self._move_window(event.position())

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._check_ifdraggable(event.position()) == True:
            self.setCursor(Qt.CursorShape.ArrowCursor)
            self._resize_window(event.position(), None)

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        if self._check_ifdraggable(event.position()) == True and event.buttons() == Qt.MouseButton.LeftButton:
            self.showNormal() if self.isMaximized() else self.showMaximized() #self.setWindowState(Qt.WindowState.WindowMaximized)

    def showEvent(self, event: QShowEvent) -> None:
        self.showed.emit()
        event.accept()

    def closeEvent(self, event: QCloseEvent) -> None:
        self.closed.emit()
        event.accept()

    def moveEvent(self, event: QMoveEvent) -> None:
        self.rectChanged.emit(self.rect())

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.rectChanged.emit(self.rect())
        self.TitleBar.resize(self.width(), self.TitleBar.height()) if isinstance(self.TitleBar, TitleBarBase) else None
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def nativeEvent(self, eventType, message):
        if eventType.type() in (QEvent.MouseButtonPress, QEvent.MouseMove):
            border_width = self.edge_size
            left   = (eventType.globalPos() - self.pos()).x() < border_width
            top    = (eventType.globalPos() - self.pos()).y() < border_width
            right  = (eventType.globalPos() - self.pos()).x() > self.width() - border_width
            bottom = (eventType.globalPos() - self.pos()).y() > self.height() - border_width
            if True not in (left, top, right, bottom):
                pass
            elif eventType.type() == QEvent.MouseButtonPress and self.windowState() != Qt.WindowNoState:
                self._move_window(eventType.globalPos())
        return False

    def setFrameless(self, SetStrechable: bool = True, SetDropShadowEffect: bool = True) -> None:
        self.setWindowFlags(self.windowFlags() | Qt.FramelessWindowHint)

    def setTitleBar(self, TitleBar: Optional[QWidget]) -> None:
        try:
            self.TitleBar.deleteLater()
            self.TitleBar.hide()
            StyleSheetBase.Bar.Deregistrate(self.TitleBar)
        except:
            pass
        if TitleBar is not None:
            self.TitleBar = TitleBar
            self.TitleBar.setParent(self) if self.TitleBar.parent() is None else None
            self.TitleBar.raise_() if self.TitleBar.isHidden() else None
        else:
            self.TitleBar = None

    def showMask(self, setVisible: bool, maskContent: Optional[str] = None) -> None:
        if setVisible:
            self.Mask.raise_() if self.Mask.isHidden() else None
            self.Mask.setText(maskContent) if maskContent is not None else self.Mask.clear()
            self.Mask.show()
        else:
            self.Mask.clear()
            self.Mask.hide()

##############################################################################################################################