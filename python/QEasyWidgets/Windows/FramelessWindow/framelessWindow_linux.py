from typing import Optional
from PySide6.QtCore import Qt, Signal, QPoint, QRect, QEvent
from PySide6.QtGui import QGuiApplication, QFont, QCursor, QMouseEvent, QShowEvent, QCloseEvent, QMoveEvent, QResizeEvent
from PySide6.QtWidgets import QApplication, QWidget, QLabel

from ...Common.Theme import BackgroundColorAnimationBase
from ...Common.StyleSheet import StyleSheetBase
from ...Components.Bar import TitleBarBase

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

        self.titleBar = TitleBarBase(self)

        self.mask = QLabel(self)
        self.rectChanged.connect(self.mask.setGeometry)
        self.mask.setStyleSheet('background-color: rgba(0, 0, 0, 111);')
        self.mask.setAlignment(Qt.AlignCenter)
        self.mask.setFont(QFont('Microsoft YaHei', int(min_height / 10), QFont.Bold))
        self.mask.hide()

        self.resize(min_width, min_height)

    def _check_ifdraggable(self, pos) -> bool:
        return (0 < pos.x() < self.width() and 0 < pos.y() < self.titleBar.height()) if self.titleBar is not None else False

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
        self.titleBar.resize(self.width(), self.titleBar.height()) if isinstance(self.titleBar, TitleBarBase) else None
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

    def setFrameless(self, setStrechable: bool = True, setDropShadowEffect: bool = True) -> None:
        self.setWindowFlags(self.windowFlags() | Qt.FramelessWindowHint)

    def setTitleBar(self, titleBar: Optional[QWidget]) -> None:
        try:
            self.titleBar.deleteLater()
            self.titleBar.hide()
            StyleSheetBase.Bar.deregistrate(self.titleBar)
        except:
            pass
        if titleBar is not None:
            self.titleBar = titleBar
            self.titleBar.setParent(self) if self.titleBar.parent() is None else None
            self.titleBar.raise_() if self.titleBar.isHidden() else None
        else:
            self.titleBar = None

    def showMask(self, setVisible: bool, maskContent: Optional[str] = None) -> None:
        if setVisible:
            self.mask.raise_() if self.mask.isHidden() else None
            self.mask.setText(maskContent) if maskContent is not None else self.mask.clear()
            self.mask.show()
        else:
            self.mask.clear()
            self.mask.hide()

##############################################################################################################################