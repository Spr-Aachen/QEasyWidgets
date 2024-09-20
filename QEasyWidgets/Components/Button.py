from typing import Optional, Union, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Icon import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################

class ButtonBase(QPushButton):
    '''
    '''
    _icon = None

    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        self.setIconSize(QSize(16, 16))

        Function_SetFont(self)

        StyleSheetBase.Button.Apply(self)

    def setIcon(self, icon: Optional[Union[QIcon, QPixmap, IconBase]]) -> None:
        if icon is not None:
            super().setProperty('hasIcon', True)
            self._icon = icon
        else:
            super().setProperty('hasIcon', False)
            self._icon = QIcon()
        super().setStyle(QApplication.style())

    def icon(self) -> QIcon:
        return Function_ToQIcon(self._icon)

    def _drawIcon(self, icon, painter, rect):
        Function_DrawIcon(icon, painter, rect)

    def paintEvent(self, e: QPaintEvent) -> None:
        super().paintEvent(e)
        if self.icon().isNull():
            return
        Width, Height = self.iconSize().width(), self.iconSize().height()
        #MinWidth, MinHeight = self.minimumSizeHint().width(), self.minimumSizeHint().height()
        LeftX = (self.width() - Width) /2
        TopY = (self.height() - Height) / 2
        Painter = QPainter(self)
        Painter.setRenderHints(QPainter.Antialiasing | QPainter.SmoothPixmapTransform)
        self._drawIcon(self._icon, Painter, QRectF(LeftX, TopY, Width, Height))

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Button.Deregistrate(self)


class MenuButton(ButtonBase):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.setIcon(IconBase.Ellipsis)

    def setMenu(self, menu: QMenu) -> None:
        def ShowMenu():
            MenuWidth = menu.sizeHint().width()
            XPos = self.width() - MenuWidth
            YPos = self.height()
            menu.exec(self.mapToGlobal(QPoint(XPos, YPos)))
        self.clicked.connect(ShowMenu)

    def SetMenu(self, ActionEvents: dict) -> None:
        Menu = QMenu(self)
        for Action in ActionEvents.keys():
            if not isinstance(Action, str):
                continue
            MenuAction = QAction(text = Action, parent = self)
            MenuAction.triggered.connect(ActionEvents.get(Action))
            Menu.addAction(MenuAction)
            #Menu.addSeparator()
        self.setMenu(Menu)


class HollowButton(ButtonBase):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)


class EmbeddedButton(ButtonBase):
    '''
    '''
    _hoverBackgroundColor = QColor(0, 0, 0, 0)

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

    def setHoverBackgroundColor(self, color: QColor) -> None:
        self._hoverBackgroundColor = color

    def setStyleSheet(self, styleSheet: str) -> None:
        super().setStyleSheet(styleSheet + "EmbeddedButton:hover {background-color: rgba%s;}" % self._hoverBackgroundColor.getRgb().__str__())

##############################################################################################################################