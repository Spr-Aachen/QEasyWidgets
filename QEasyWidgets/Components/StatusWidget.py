import math
import time
from PySide6.QtCore import Qt, QTimer, QPointF, QSize
from PySide6.QtGui import QPainter, QColor, QPaintEvent, QResizeEvent
from PySide6.QtWidgets import QWidget, QGridLayout

#from ..Common.StyleSheet import *
from ..Common.Status import Status

##############################################################################################################################

class LoadingStatus(QWidget):
    locationList = []
    radiiList = []

    def __init__(self, dotcount: int = 12, interval: int = 50, parent = None):
        super().__init__(parent)

        self.setAttribute(Qt.WA_TranslucentBackground, True)

        self.setDotColor(QColor(48, 177, 222))
        self.setDotCount(dotcount)

        self.timer = QTimer()
        self.timer.setInterval(interval)
        self.timer.timeout.connect(self.repaint)
        self.timer.start()

    def setDotCount(self, count: int):
        self._count = count

    def setDotColor(self, color: QColor):
        self._dotColor = color

    def resizeEvent(self, event: QResizeEvent):
        _squareWidth = min(self.width(), self.height())
        _maxDiameter = _squareWidth / 6
        _minDiameter = _maxDiameter - _squareWidth/12
        half = _squareWidth / 2
        _centerDistance = half - _maxDiameter/2 - 1
        gap = (_maxDiameter - _minDiameter) / (self._count-1) / 2
        angleGap = 360 / self._count
        self.locationList.clear()
        self.radiiList.clear()
        for i in range(self._count):
            self.radiiList.append(_maxDiameter/2 - i*gap)
            radian = math.radians(- angleGap*i)
            self.locationList.append((half + _centerDistance*math.cos(radian), half - _centerDistance*math.sin(radian)))

    def _paintDot(self, painter: QPainter):
        for index in range(self._count):
            painter.setPen(self._dotColor)
            radii = self.radiiList[(index + int(time.time() * 1000 / self.timer.interval())) % self._count]
            painter.drawEllipse(QPointF(*self.locationList[index]), radii, radii)

    def paintEvent(self, event: QPaintEvent):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setPen(self._dotColor)
        painter.setBrush(self._dotColor)
        self._paintDot(painter)

##############################################################################################################################

class StatusWidgetBase(QWidget):
    currentStatus = None
    def __init__(self, status: Status = None, size: QSize = QSize(24, 24), parent = None):
        super().__init__(parent)

        self.setAttribute(Qt.WA_TranslucentBackground, True)

        self.Layout = QGridLayout(self)
        self.Layout.setContentsMargins(0, 0, 0, 0)
        self.Layout.setSpacing(0)
    
        self.setStatus(status)

        self.setFixedSize(size)

    def setStatus(self, status):
        if status is None:
            try:
                self.deleteLater()
            except:
                pass
            return
        self.Layout.removeWidget(self.currentStatus) if self.currentStatus is not None else None
        if status == Status.Loading:
            self.currentStatus = LoadingStatus(parent = self)
        self.Layout.addWidget(self.currentStatus)

##############################################################################################################################