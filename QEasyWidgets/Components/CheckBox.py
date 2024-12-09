# coding: utf-8
from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.Theme import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class Indicator(QPushButton):
    """
    """
    isPressed = False
    isHover = False

    _width = 48
    _ellipseLength = 12

    checked = Signal(bool)

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setCheckable(True)

        self._height = self._width // 2
        self.setFixedSize(self._width, self._height)

        self._ellipseCordX = self._ellipseLength // 2
        self._ellipseCordX_Default = self._ellipseCordX

        self.ellipseAnimation = QPropertyAnimation(self, b'ellipseCordX', self)
        self.ellipseAnimation.setDuration(123)

        self.toggled.connect(lambda: (
            self.ellipseAnimation.setEndValue((self._width - self._ellipseLength - self._ellipseCordX_Default) if self.isChecked() else self._ellipseCordX_Default),
            self.ellipseAnimation.start()
        ))

    def toggle(self):
        self.setChecked(not self.isChecked())

    def mouseReleaseEvent(self, e):
        super().mouseReleaseEvent(e)
        self.checked.emit(self.isChecked())

    def setDown(self, isDown: bool):
        self.isPressed = isDown
        super().setDown(isDown)

    def setHover(self, isHover: bool):
        self.isHover = isHover
        self.update()

    def getEllipseCordX(self):
        return self._ellipseCordX

    def setEllipseCordX(self, x):
        self._ellipseCordX = max(x, self._ellipseCordX_Default)
        self.update()

    ellipseCordX = Property(float, getEllipseCordX, setEllipseCordX)

    def paintEvent(self, e):
        painter = QPainter(self)
        painter.setRenderHints(QPainter.Antialiasing)
        # Draw background
        radius = self.height() / 2
        painter.setPen(ThemeColor.Default.color() if self.isChecked() else (ThemeColor.Dark.color() if EasyTheme.THEME == Theme.Dark else ThemeColor.Light.color()))
        painter.setBrush(ThemeColor.Default.color() if self.isChecked() else QColor(Qt.white if EasyTheme.THEME == Theme.Dark else Qt.black))
        painter.drawRoundedRect(self.rect().adjusted(1, 1, -1, -1), radius, radius)
        # Draw circle
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor(Qt.black if EasyTheme.THEME == Theme.Dark else Qt.white) if self.isChecked() else (ThemeColor.Dark.color() if EasyTheme.THEME == Theme.Dark else ThemeColor.Light.color()))
        painter.drawEllipse(int(self.ellipseCordX), (self._height - self._ellipseLength) // 2, self._ellipseLength, self._ellipseLength)


class CheckBoxBase(QCheckBox):
    """
    Base class for checkBox components
    """
    _spacing = 12

    toggled = Signal(bool)

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent = parent)

        setFont(self, 15)

        self.indicator = Indicator(self)
        self.indicator.toggled.connect(self.toggled.emit)

        self.label = QLabel(self)

        # set layout
        layout = QHBoxLayout(self)
        layout.setContentsMargins(3, 0, 0, 0)
        layout.setSpacing(self._spacing)
        layout.addWidget(self.indicator)
        layout.addWidget(self.label)
        layout.setAlignment(Qt.AlignLeft)

        self.setMinimumSize(layout.totalSizeHint())

        StyleSheetBase.CheckBox.Apply(self)

    @__init__.register
    def _(self, text: str = ..., parent: QWidget = None):
        self.__init__(parent)
        self.setText(text)

    def eventFilter(self, watched: QObject, event: QEvent):
        if watched is self and self.isEnabled():
            if event.type() == QEvent.MouseButtonPress:
                self.indicator.setDown(True)
            elif event.type() == QEvent.MouseButtonRelease:
                self.indicator.setDown(False)
                self.indicator.toggle()
            if event.type() == QEvent.Enter:
                self.indicator.setHover(True)
            elif event.type() == QEvent.Leave:
                self.indicator.setHover(False)
        return super().eventFilter(watched, event)

    def text(self):
        return self.label.text()

    def setText(self, text):
        self.label.setText(text)
        self.adjustSize()

    def isChecked(self):
        return self.indicator.isChecked()

    def setChecked(self, isChecked):
        self.indicator.setChecked(isChecked)

    def getSpacing(self):
        return self._spacing

    def setSpacing(self, spacing):
        self._spacing = spacing
        self.layout().setSpacing(spacing)
        self.update()

    spacing = Property(int, getSpacing, setSpacing)

##############################################################################################################################