import darkdetect
from enum import Enum
from typing import Union
from PySide6.QtCore import QEvent, QObject, QPropertyAnimation, Property
from PySide6.QtGui import Qt, QColor, QPainter

from .Signals import ComponentsSignals

##############################################################################################################################

class Theme:
    """
    """
    Dark = 'Dark'
    Light = 'Light'

    Auto = darkdetect.theme()


class ThemeBase:
    """
    """
    THEME = Theme.Auto if Theme.Auto is not None else Theme.Dark

    def update(self, theme: str):
        if theme in (Theme.Dark, Theme.Light):
            self.THEME = theme


EasyTheme = ThemeBase()


def currentTheme():
    return EasyTheme.THEME


def isDarkTheme():
    return currentTheme() == Theme.Dark

##############################################################################################################################

class ThemeColor(Enum):
    """
    """
    Default = 'DefaultThemeColor'
    Light = 'LightThemeColor'
    Dark = 'DarkThemeColor'

    def color(self):
        if self == self.Light:
            return QColor(246, 246, 246)
        elif self == self.Dark:
            return QColor(24, 24, 24)
        return QColor(120, 180, 240, 123)


def currentColor():
    return ThemeColor.Dark.color() if isDarkTheme() else ThemeColor.Light.color()

##############################################################################################################################

class BackgroundColorObject(QObject):
    """
    Background color object
    """
    def __init__(self, parent):
        super().__init__(parent)

        self._backgroundColor = parent._normalBackgroundColor()

    @Property(QColor)
    def backgroundColor(self):
        return self._backgroundColor

    @backgroundColor.setter
    def backgroundColor(self, color: QColor):
        self._backgroundColor = color
        self.parent().update()


class BackgroundColorAnimationBase:
    """
    Background color animation base for widgets
    """
    _lightBackgroundColor = ThemeColor.Light.color()
    _darkBackgroundColor = ThemeColor.Dark.color()

    isHover = False
    isPressed = False

    def __init__(self, *args, **kwargs) -> None:
        self.installEventFilter(self)

        self.bgColorObject = BackgroundColorObject(self)

        self.bgColorAnim = QPropertyAnimation(self.bgColorObject, b'backgroundColor', self)
        self.bgColorAnim.setDuration(210)

        ComponentsSignals.Signal_SetTheme.connect(self._updateBackgroundColor)

    def _normalBackgroundColor(self):
        return self._darkBackgroundColor if isDarkTheme() else self._lightBackgroundColor

    def _hoverBackgroundColor(self):
        return self._normalBackgroundColor()

    def _pressedBackgroundColor(self):
        return self._normalBackgroundColor()

    def _focusInBackgroundColor(self):
        return self._normalBackgroundColor()

    def _disabledBackgroundColor(self):
        return self._normalBackgroundColor()

    def _updateBackgroundColor(self):
        if not self.isEnabled():
            color = self._disabledBackgroundColor()
        elif hasattr(self, 'hasFocus') and self.hasFocus():
            color = self._focusInBackgroundColor()
        elif self.isPressed:
            color = self._pressedBackgroundColor()
        elif self.isHover:
            color = self._hoverBackgroundColor()
        else:
            color = self._normalBackgroundColor()
        self.bgColorAnim.stop()
        self.bgColorAnim.setEndValue(color)
        self.bgColorAnim.start()

    def setBackgroundColor(self, color: Union[QColor, str, int]):
        self.bgColorObject.backgroundColor = QColor(color)

    def getBackgroundColor(self):
        return self.bgColorObject.backgroundColor

    @property
    def backgroundColor(self):
        return self.getBackgroundColor()

    def eventFilter(self, obj, e):
        if obj is self and e.type() == QEvent.Type.EnabledChange:
            self.setBackgroundColor(self._normalBackgroundColor() if self.isEnabled() else self._disabledBackgroundColor())
        return super().eventFilter(obj, e)

    def paintEvent(self, e):
        painter = QPainter(self)
        painter.setPen(Qt.NoPen)
        painter.setBrush(self.backgroundColor)
        painter.drawRect(self.rect())
        super().paintEvent(e)

    def mousePressEvent(self, e):
        self.isPressed = True
        self._updateBackgroundColor()
        super().mousePressEvent(e)

    def mouseReleaseEvent(self, e):
        self.isPressed = False
        self._updateBackgroundColor()
        super().mouseReleaseEvent(e)

    def enterEvent(self, e):
        self.isHover = True
        self._updateBackgroundColor()

    def leaveEvent(self, e):
        self.isHover = False
        self._updateBackgroundColor()

    def focusInEvent(self, e):
        super().focusInEvent(e)
        self._updateBackgroundColor()

    def setCustomBackgroundColor(self, light: Union[QColor, str, int], dark: Union[QColor, str, int]):
        self._lightBackgroundColor = QColor(light)
        self._darkBackgroundColor = QColor(dark)
        self._updateBackgroundColor()

##############################################################################################################################