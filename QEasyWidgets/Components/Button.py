from typing import Optional, Union, overload
#from functools import singledispatchmethod
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
    _spacing = 3

    _hoverBackgroundColor = QColor(0, 0, 0, 0)

    _icon = None

    _alignment = Qt.AlignCenter

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None) -> None:
        super().__init__(parent)

        self.setIconSize(QSize(16, 16))

        Function_SetFont(self, 12)

        StyleSheetBase.Button.Apply(self)

    @__init__.register
    def _(self, text: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setText(text)

    @__init__.register
    def _(self, icon: Union[QIcon, QPixmap], text: str, parent: Optional[QWidget] = None) -> None:
        self.__init__(parent)
        self.setIcon(icon)
        self.setText(text)

    def setSpacing(self, spacing: int) -> None:
        self._spacing = spacing

    def spacing(self) -> int:
        return self._spacing

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

    def setAlignment(self, alignment: Qt.AlignmentFlag) -> None:
        self._alignment = alignment

    def alignment(self) -> Qt.AlignmentFlag:
        return self._alignment

    def minimumSizeHint(self):
        icon_size = self.iconSize() if not self.icon().isNull() else None
        text_width = self.fontMetrics().horizontalAdvance(self.text()) if self.text().__len__() > 0 else None
        return QSize(
            ((self.spacing() + icon_size.width()) if icon_size else 0) + self.spacing() + ((text_width + self.spacing()) if text_width else 0),
            (max(icon_size.height(), self.fontMetrics().height()) if icon_size else self.fontMetrics().height()) + self.spacing() // 2
        )

    def paintEvent(self, e: QPaintEvent) -> None:
        icon_size = self.iconSize() if not self.icon().isNull() else None
        text_width = self.fontMetrics().horizontalAdvance(self.text()) if self.text().__len__() > 0 else None
        painter = QStylePainter(self)
        painter.setRenderHints(QPainter.Antialiasing | QPainter.SmoothPixmapTransform)
        # Draw background
        option = QStyleOptionButton()
        self.initStyleOption(option)
        self.style().drawPrimitive(QStyle.PE_Widget, option, painter, self)
        # Calculate content rect
        content_width = self.minimumSizeHint().width()
        content_height = self.minimumSizeHint().height()
        content_rect = QRect(
            (self.rect().center().x() - content_width // 2) if self.property("isHorizontal") == True else 0,
            self.rect().center().y() - content_height // 2,
            content_width,
            content_height
        )
        content_rect.moveCenter(self.rect().center()) if self.alignment() == Qt.AlignCenter else None
        # Draw icon
        icon_rect = QRect(
            content_rect.left() + self.spacing(),
            content_rect.top(),
            icon_size.width(),
            icon_size.height()
        ) if icon_size else None
        self._drawIcon(self._icon, painter, icon_rect) if icon_size else None
        # Draw text
        text_rect = QRect(
            (icon_rect.right() if icon_size else content_rect.left()) + self.spacing(),
            content_rect.top(),
            text_width,
            content_rect.height()
        ) if text_width else None
        painter.drawText(text_rect, Qt.AlignLeft | Qt.AlignVCenter, self.text()) if text_width else None

    def setStyleSheet(self, styleSheet: str) -> None:
        super().setStyleSheet(styleSheet + "ButtonBase:hover {background-color: rgba%s;}" % self._hoverBackgroundColor.getRgb().__str__())

    def setHoverBackgroundColor(self, color: QColor) -> None:
        self._hoverBackgroundColor = color

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def setTransparent(self, transparent: bool) -> None:
        self.setProperty("isTransparent", transparent)

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


class NavigationButton(ButtonBase):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.setCheckable(True)
        self.setAutoExclusive(True)

        self.setSpacing(13.5)
        self.setAlignment(Qt.AlignLeft)
        self.setHorizontal(False)

    def setHorizontal(self, horizontal: bool) -> None:
        self.setProperty("isHorizontal", horizontal)

##############################################################################################################################