from typing import Optional, overload
from PyEasyUtils import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.Theme import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class ArrowButton(QToolButton):
    """
    Arrow button
    """
    def __init__(self, icon: IconBase, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setFixedSize(10, 10)
        self._icon = icon

    def paintEvent(self, e):
        painter = QPainter(self)
        painter.setRenderHints(QPainter.Antialiasing)

        length = 6 if self.isDown() else 9
        cord = (self.width() - length) / 2
        self._icon.paint(painter, QRectF(cord, cord, length, length), Theme.Auto)


class ScrollBarGroove(QWidget):
    """
    Scroll bar groove
    """
    def __init__(self, orient: Qt.Orientation, parent: QWidget):
        super().__init__(parent)

        if orient == Qt.Vertical:
            self.setFixedWidth(12)
            self.upButton = ArrowButton(IconBase.Chevron_Up, self)
            self.downButton = ArrowButton(IconBase.Chevron_Down, self)
            layout = QVBoxLayout(self)
            layout.addWidget(self.upButton, 0, Qt.AlignHCenter)
            layout.addStretch(1)
            layout.addWidget(self.downButton, 0, Qt.AlignHCenter)
            layout.setContentsMargins(0, 3, 0, 3)
        if orient == Qt.Horizontal:
            self.setFixedHeight(12)
            self.upButton = ArrowButton(IconBase.Chevron_Left, self)
            self.downButton = ArrowButton(IconBase.Chevron_Right, self)
            layout = QHBoxLayout(self)
            layout.addWidget(self.upButton, 0, Qt.AlignVCenter)
            layout.addStretch(1)
            layout.addWidget(self.downButton, 0, Qt.AlignVCenter)
            layout.setContentsMargins(3, 0, 3, 0)

        self.opacityEffect = QGraphicsOpacityEffect(self)
        self.opacityAnimation = QPropertyAnimation(self.opacityEffect, b'opacity', self)
        self.setGraphicsEffect(self.opacityEffect)
        self.opacityEffect.setOpacity(0)

    def paintEvent(self, e):
        painter = QPainter(self)
        painter.setRenderHints(QPainter.Antialiasing)
        painter.setPen(Qt.NoPen)

        if not isDarkTheme():
            painter.setBrush(QColor(246, 246, 246, 234))
        else:
            painter.setBrush(QColor(48, 48, 48, 234))

        painter.drawRoundedRect(self.rect(), 6, 6)

    def fadeIn(self):
        self.opacityAnimation.setEndValue(1)
        self.opacityAnimation.setDuration(150)
        self.opacityAnimation.start()

    def fadeOut(self):
        self.opacityAnimation.setEndValue(0)
        self.opacityAnimation.setDuration(150)
        self.opacityAnimation.start()


class ScrollBarHandle(QWidget):
    """
    Scroll bar handle
    """
    def __init__(self, orient: Qt.Orientation, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.orient = orient

        if self.orient == Qt.Vertical:
            self.setFixedWidth(3)
        if self.orient == Qt.Horizontal:
            self.setFixedHeight(3)

    def paintEvent(self, e):
        painter = QPainter(self)
        painter.setRenderHints(QPainter.Antialiasing)
        painter.setPen(Qt.NoPen)

        radius = self.width() / 2 if self.orient == Qt.Vertical else self.height() / 2
        color = QColor(255, 255, 255, 123) if isDarkTheme() else QColor(0, 0, 0, 123)
        painter.setBrush(color)
        painter.drawRoundedRect(self.rect(), radius, radius)


class ScrollBar(QWidget):
    """
    Fluent scroll bar
    """
    rangeChanged = Signal(tuple)
    valueChanged = Signal(int)

    sliderPressed = Signal()
    sliderReleased = Signal()
    sliderMoved = Signal()

    def __init__(self, arg__1: Qt.Orientation, parent: QAbstractScrollArea = ...) -> None:
        super().__init__(parent)

        parent.installEventFilter(self)

        self.timer = QTimer(self)

        self._minimum = 0
        self._maximum = 0
        self._value = 0

        self._singleStep = 1
        self._pageStep = 30
        self._padding = 15

        self._pressedPos = QPoint()
        self._isPressed = False
        self._isEnter = False
        self._isExpanded = False
        self._isAlwaysOff = False

        self._orientation = arg__1

        if self._orientation == Qt.Vertical:
            parentScrollBar = parent.verticalScrollBar()
            QAbstractScrollArea.setVerticalScrollBarPolicy(parent, Qt.ScrollBarAlwaysOff)
        else:
            parentScrollBar = parent.horizontalScrollBar()
            QAbstractScrollArea.setHorizontalScrollBarPolicy(parent, Qt.ScrollBarAlwaysOff)
        parentScrollBar.rangeChanged.connect(self.setRange)
        parentScrollBar.valueChanged.connect(self._onValueChanged)
        self.valueChanged.connect(parentScrollBar.setValue)

        self.groove = ScrollBarGroove(self._orientation, self)
        self.groove.upButton.clicked.connect(self._onPageUp)
        self.groove.downButton.clicked.connect(self._onPageDown)
        self.groove.opacityAnimation.valueChanged.connect(self._onOpacityAniValueChanged)

        self.handle = ScrollBarHandle(self._orientation, self)

        self.setRange(parentScrollBar.minimum(), parentScrollBar.maximum())
        self.setVisible(self.maximum() > 0 and not self._isAlwaysOff)
        self._adjustPos(self.parent().size())

        self.duration = 369
        self.ani = QPropertyAnimation()
        self.ani.setTargetObject(self)
        self.ani.setPropertyName(b"val")
        self.ani.setEasingCurve(QEasingCurve.OutCubic)
        self.ani.setDuration(self.duration)

    def minimum(self):
        return self._minimum

    def maximum(self):
        return self._maximum

    def setMinimum(self, min: int):
        if min == self.minimum():
            return

        self._minimum = min
        self.rangeChanged.emit((min, self.maximum()))

    def setMaximum(self, max: int):
        if max == self.maximum():
            return

        self._maximum = max
        self.rangeChanged.emit((self.minimum(), max))

    def setRange(self, min: int, max: int):
        if min > max or (min == self.minimum() and max == self.maximum()):
            return

        self.setMinimum(min)
        self.setMaximum(max)

        self._adjustHandleSize()
        self._adjustHandlePos()
        self.setVisible(max > 0 and not self._isAlwaysOff)

        self.rangeChanged.emit((min, max))

    def pageStep(self):
        return self._pageStep

    def singleStep(self):
        return self._singleStep

    def setPageStep(self, step: int):
        if step >= 1:
            self._pageStep = step

    def setSingleStep(self, step: int):
        if step >= 1:
            self._singleStep = step

    def orientation(self):
        return self._orientation

    def _adjustHandleSize(self):
        p = self.parent()
        if self.orientation() == Qt.Vertical:
            total = self.maximum() - self.minimum() + p.height()
            s = int(self._grooveLength() * p.height() / max(total, 1))
            self.handle.setFixedHeight(max(30, s))
        else:
            total = self.maximum() - self.minimum() + p.width()
            s = int(self._grooveLength() * p.width() / max(total, 1))
            self.handle.setFixedWidth(max(30, s))

    def _adjustHandlePos(self):
        total = max(self.maximum() - self.minimum(), 1)
        delta = int(self.value() / total * self._slideLength())

        if self.orientation() == Qt.Vertical:
            x = self.width() - self.handle.width() - 3
            self.handle.move(x, self._padding + delta)
        else:
            y = self.height() - self.handle.height() - 3
            self.handle.move(self._padding + delta, y)

    @Property(int, notify = valueChanged)
    def val(self):
        return self._value

    @val.setter
    def val(self, value: int):
        if value == self.value():
            return

        value = max(self.minimum(), min(value, self.maximum()))
        self._value = value
        self.valueChanged.emit(value)

        self._adjustHandlePos()

    def _onPageUp(self):
        self.setValue(self.value() - self.pageStep())

    def _onPageDown(self):
        self.setValue(self.value() + self.pageStep())

    def _onOpacityAniValueChanged(self):
        opacity = self.groove.opacityEffect.opacity()
        if self.orientation() == Qt.Vertical:
            self.handle.setFixedWidth(int(3 + opacity * 3))
        else:
            self.handle.setFixedHeight(int(3 + opacity * 3))

        self._adjustHandlePos()

    def _onValueChanged(self, value):
        self.val = value

    def value(self):
        return self._value

    def setValue(self, value: int):
        if value == self.value():
            return

        distance = abs(value - self.value())
        duration = min(500, max(200, int(distance * 5)))

        self.ani.setDuration(duration)
        self.ani.setStartValue(self.value())
        self.ani.setEndValue(value)
        self.ani.start()

    def setValueImmediately(self, value: int):
        self.ani.stop()
        if value == self.value():
            return

        value = max(self.minimum(), min(value, self.maximum()))
        self._value = value
        self.valueChanged.emit(value)
        self._adjustHandlePos()

    def isSliderDown(self):
        return self._isPressed

    def setSliderDown(self, isDown: bool):
        self._isPressed = True
        if isDown:
            self.sliderPressed.emit()
        else:
            self.sliderReleased.emit()

    def expand(self):
        if self._isExpanded or not self.isEnter:
            return

        self._isExpanded = True
        self.groove.fadeIn()

    def collapse(self):
        if not self._isExpanded or self.isEnter:
            return

        self._isExpanded = False
        self.groove.fadeOut()

    def enterEvent(self, e):
        self.isEnter = True
        self.timer.stop()
        self.timer.singleShot(210, self.expand)

    def leaveEvent(self, e):
        self.isEnter = False
        self.timer.stop()
        self.timer.singleShot(210, self.collapse)

    def _adjustPos(self, size):
        if self.orientation() == Qt.Vertical:
            self.resize(12, size.height() - 2)
            self.move(size.width() - 13, 1)
        else:
            self.resize(size.width() - 2, 12)
            self.move(1, size.height() - 13)

    def eventFilter(self, obj, e: QEvent):
        if obj is not self.parent():
            return super().eventFilter(obj, e)

        if e.type() == QEvent.Resize:
            self._adjustPos(e.size())

        return super().eventFilter(obj, e)

    def resizeEvent(self, e):
        self.groove.resize(self.size())

    def _grooveLength(self):
        if self.orientation() == Qt.Vertical:
            return self.height() - 2 * self._padding

        return self.width() - 2 * self._padding

    def _slideLength(self):
        if self.orientation() == Qt.Vertical:
            return self._grooveLength() - self.handle.height()

        return self._grooveLength() - self.handle.width()

    def _isSlideResion(self, pos: QPoint):
        if self.orientation() == Qt.Vertical:
            return self._padding <= pos.y() <= self.height() - self._padding

        return self._padding <= pos.x() <= self.width() - self._padding

    def _jumpToPositionWithAnimation(self, pos):
        if self.orientation() == Qt.Vertical:
            groove = self._grooveLength()
            slide = self._slideLength()
            posY = pos.y() - self._padding
            posY = max(0, min(posY, groove))
            if posY > self.handle.y() + self.handle.height()/2:
                posY -= self.handle.height()
            value = int(posY / max(slide, 1) * self.maximum())
        else:
            groove = self._grooveLength()
            slide = self._slideLength()
            posX = pos.x() - self._padding
            posX = max(0, min(posX, groove))
            if posX > self.handle.x() + self.handle.width()/2:
                posX -= self.handle.width()
            value = int(posX / max(slide, 1) * self.maximum())

        self.setValue(value)

    def mousePressEvent(self, e: QMouseEvent):
        self.ani.stop()

        self._isPressed = True
        self._pressedPos = e.pos()
        self._handlePressed = self.childAt(e.pos()) is self.handle

        if not self._handlePressed and self._isSlideResion(e.pos()):
            self._jumpToPositionWithAnimation(e.pos())
            self.sliderPressed.emit()
        elif self._handlePressed:
            self._pressedValue = self.value()
            self.sliderPressed.emit()

    def mouseReleaseEvent(self, e):
        super().mouseReleaseEvent(e)
        self._isPressed = False
        self.sliderReleased.emit()

    def mouseMoveEvent(self, e: QMouseEvent):
        if not self._isPressed or not self._handlePressed:
            return

        self.ani.stop()

        if self.orientation() == Qt.Vertical:
            dv = e.pos().y() - self._pressedPos.y()
            delta = int(dv / max(self._slideLength(), 1) * (self.maximum() - self.minimum()))
        else:
            dv = e.pos().x() - self._pressedPos.x()
            delta = int(dv / max(self._slideLength(), 1) * (self.maximum() - self.minimum()))

        new_value = self._pressedValue + delta
        new_value = max(self.minimum(), min(new_value, self.maximum()))
        self.setValueImmediately(new_value)

        self.sliderMoved.emit()

    def wheelEvent(self, e: QWheelEvent):
        self.ani.stop()

        delta = e.angleDelta().y() or e.angleDelta().x()
        if delta == 0:
            return

        new_value = self.value() - delta
        new_value = max(self.minimum(), min(new_value, self.maximum()))

        self.setValueImmediately(new_value)
        e.accept()

    def setScrollValue(self, value):
        self._value += value
        self._value = max(self.minimum(), self._value)
        self._value = min(self.maximum(), self._value)
        self.setValue(self._value)

    def scrollTo(self, value):
        self._value = value
        self._value = max(self.minimum(), self._value)
        self._value = min(self.maximum(), self._value)
        self.setValue(self._value)

    def setAlwaysOff(self, alwaysOff: bool):
        self._isAlwaysOff = alwaysOff
        self.setVisible(not alwaysOff and self.maximum() > 0)

##############################################################################################################################

class ScrollDelegate(QObject):
    """
    Scroll delegate
    """
    def __init__(self, parent: QAbstractScrollArea):
        super().__init__(parent)

        self.vScrollBar = ScrollBar(Qt.Vertical, parent)
        self.hScrollBar = ScrollBar(Qt.Horizontal, parent)

        if isinstance(parent, QAbstractItemView):
            parent.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
            parent.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
        if isinstance(parent, QListView):
            parent.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOn)
            parent.horizontalScrollBar().setStyleSheet("QScrollBar:horizontal{height: 0px}")

        parent.viewport().installEventFilter(self)
        parent.setVerticalScrollBarPolicy = self.setVerticalScrollBarPolicy
        parent.setHorizontalScrollBarPolicy = self.setHorizontalScrollBarPolicy

    def eventFilter(self, obj, e: QEvent):
        if e.type() == QEvent.Wheel:
            if e.angleDelta().y() != 0:
                self.vScrollBar.setScrollValue(-e.angleDelta().y())
            else:
                self.hScrollBar.setScrollValue(-e.angleDelta().x())

            e.setAccepted(True)
            return True

        return super().eventFilter(obj, e)

    def setVerticalScrollBarPolicy(self, policy):
        QAbstractScrollArea.setVerticalScrollBarPolicy(self.parent(), Qt.ScrollBarAlwaysOff)
        self.vScrollBar.setAlwaysOff(policy == Qt.ScrollBarAlwaysOff)

    def setHorizontalScrollBarPolicy(self, policy):
        QAbstractScrollArea.setHorizontalScrollBarPolicy(self.parent(), Qt.ScrollBarAlwaysOff)
        self.hScrollBar.setAlwaysOff(policy == Qt.ScrollBarAlwaysOff)

##############################################################################################################################

class ScrollAreaBase(QScrollArea):
    """
    Base class for scrollArea components
    """
    viewportSizeChanged = Signal(QSize)

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setWidgetResizable(True)

        self.scrollDelegate = ScrollDelegate(self)

        StyleSheetBase.ScrollArea.apply(self)

    def setWidget(self, widget: QWidget) -> None:
        self.widget().deleteLater() if self.widget() is not None else None
        super().setWidget(widget)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.viewportSizeChanged.emit(self.viewport().size())
        super().resizeEvent(event)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ScrollArea.deregistrate(self)


class VerticalScrollArea(ScrollAreaBase):
    """
    Vertical scrollArea component
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.viewportSizeChanged.connect(self.onViewportSizeChanged)

    def onViewportSizeChanged(self, size: QSize) -> None:
        self.widget().setMaximumWidth(size.width()) if self.widget() is not None else None

##############################################################################################################################