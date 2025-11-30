from typing import Optional, overload
from PyEasyUtils import singledispatchmethod
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

from ..Common.Config import Position
from ..Common.StyleSheet import StyleSheetBase
from ..Common.QFunctions import getCurrentScreen, getScreenGeometry, setDropShadowEffect, setOpacityEffect

##############################################################################################################################

class ToolTipBase(QFrame):
    """
    Base class for toolTip components
    """
    def __init__(self, parent: QWidget = ..., text = '',):
        super().__init__(parent)

        self._text = text

        self._duration = 333

        self.timer = QTimer(self)
        self.timer.setSingleShot(True)
        self.timer.timeout.connect(self.hide)

        # set layout
        self.setLayout(QHBoxLayout())
        self.layout().setContentsMargins(12, 8, 12, 12)

        # set container
        self.container = self._createContainer()
        self.containerLayout = QHBoxLayout(self.container)
        self.layout().addWidget(self.container)
        self.label = QLabel(text, self)
        self.containerLayout.addWidget(self.label)
        self.containerLayout.setContentsMargins(8, 6, 8, 6)

        # add shadow
        setDropShadowEffect(self,
            blurRadius = 21,
            color = QColor(0, 0, 0, 60),
            offset_dx = 0, offset_dy = 3,
        )

        # add opacity effect
        self.opacityAnim = setOpacityEffect(self,)

        # set style
        self.setAttribute(Qt.WA_TransparentForMouseEvents)
        self.setAttribute(Qt.WA_TranslucentBackground)
        self.setWindowFlags(Qt.Tool | Qt.FramelessWindowHint)
        self._setQSS()

    def _createContainer(self):
        return QFrame(self)

    def _setQSS(self):
        self.container.setObjectName("container")
        self.label.setObjectName("contentLabel")
        StyleSheetBase.ToolTip.apply(self)
        self.label.adjustSize()
        self.adjustSize()

    def showEvent(self, e):
        self.opacityAnim.setStartValue(0)
        self.opacityAnim.setEndValue(1)
        self.opacityAnim.start()
        self.timer.stop()
        if self.duration() > 0:
            self.timer.start(self._duration + self.opacityAnim.duration())
        super().showEvent(e)

    def text(self):
        return self._text

    def setText(self, text):
        self._text = text
        self.label.setText(text)
        self.container.adjustSize()
        self.adjustSize()

    @singledispatchmethod
    def showText(self, pos: QPoint, text: str):
        self.move(pos)
        self.setText(text)
        self.show()

    @showText.register
    def _(self, x: int, y: int, text: str):
        self.move(x, y)
        self.show(text)

    def hideText(self):
        self.hide()

    def hideEvent(self, e):
        self.timer.stop()
        super().hideEvent(e)

    def duration(self):
        return self._duration

    def setDuration(self, duration: int):
        self._duration = duration

    def adjustPos(self, widget: QWidget, position: Position):
        if position == Position.Top:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() + widget.width()//2 - self.width()//2
            y = pos.y() - self.height()
        elif position == Position.Bottom:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() + widget.width()//2 - self.width()//2
            y = pos.y() + widget.height()
        elif position == Position.Left:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() - self.width()
            y = pos.y() + (widget.height() - self.height()) // 2
        elif position == Position.Right:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() + widget.width()
            y = pos.y() + (widget.height() - self.height()) // 2
        elif position == Position.TopLeft:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() - self.layout().contentsMargins().left()
            y = pos.y() - self.height()
        elif position == Position.TopRight:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() + widget.width() - self.width() + self.layout().contentsMargins().right()
            y = pos.y() - self.height()
        elif position == Position.BottomLeft:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() - self.layout().contentsMargins().left()
            y = pos.y() + widget.height()
        elif position == Position.BottomRight:
            pos = widget.mapToGlobal(QPoint())
            x = pos.x() + widget.width() - self.width() + self.layout().contentsMargins().right()
            y = pos.y() + widget.height()
        rect = getScreenGeometry(getCurrentScreen())
        x = max(rect.left(), min(pos.x(), rect.right() - self.width() - 4))
        y = max(rect.top(), min(pos.y(), rect.bottom() - self.height() - 4))
        self.move(x, y)


class ToolTipEventFilter(QObject):
    """    
    """
    def __init(self, parent: Optional[QWidget] = None, delay: int = 333):
        super().__init__(parent)

        self._showDelay = delay

        self.isEnter = False

        self.timer = QTimer(self)
        self.timer.setSingleShot(True)
        self.timer.timeout.connect(self.showToolTip)

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None, position = Position.Top, delay: int = 333):
        self.__init(parent, delay)

        self.position = position

        self._toolTip = ToolTipBase(self.parent().window(), self.parent().toolTip())

    @__init__.register
    def _(self, parent: Optional[QWidget] = None, toolTip: ToolTipBase = ..., delay: int = 333):
        self.__init(parent, delay)

        self._toolTip = toolTip if isinstance(toolTip, ToolTipBase) else ToolTipBase(self.parent().window(), self.parent().toolTip())

    def eventFilter(self, obj: QObject, e: QEvent) -> bool:
        if e.type() == QEvent.ToolTip:
            return True
        elif e.type() in [QEvent.Hide, QEvent.Leave]:
            self.hideToolTip()
        elif e.type() == QEvent.Enter:
            self.isEnter = True
            parent = self.parent()
            if parent.isWidgetType() and parent.toolTip() and parent.isEnabled():
                t = parent.toolTipDuration() if parent.toolTipDuration() > 0 else -1
                self._toolTip.setDuration(t)
                self.timer.start(self._showDelay) # show toolTip after delay
        elif e.type() == QEvent.MouseButtonPress:
            self.hideToolTip()
        return super().eventFilter(obj, e)

    def hideToolTip(self):
        self.isEnter = False
        self.timer.stop()
        if self._toolTip:
            self._toolTip.hide()

    def showToolTip(self):
        if not self.isEnter:
            return

        parent = self.parent()
        self._toolTip.setText(parent.toolTip())
        self._toolTip.adjustPos(parent, self.position)
        self._toolTip.show()

    def setToolTipDelay(self, delay: int):
        self._showDelay = delay
    
##############################################################################################################################