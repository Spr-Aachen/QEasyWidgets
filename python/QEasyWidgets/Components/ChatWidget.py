from typing import Optional, overload
from PyEasyUtils import singledispatchmethod
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Config import ChatRole
from ..Common.StyleSheet import *
from .Frame import FrameBase
from .StatusWidget import StatusWidgetBase
from .ScrollArea import VerticalScrollArea

##############################################################################################################################

class AvatarDisplay(QLabel):
    """
    """
    clicked = Signal()

    @singledispatchmethod
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

    @__init__.register
    def _(self, size: QSize, avatar: Union[str, QPixmap], parent: Optional[QWidget] = None):
        self.__init__(parent)
        self.setAvatar(avatar, size)
        self.setFixedSize(size)

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        super().mouseDoubleClickEvent(event)
        self.clicked.emit()

    def setAvatar(self, avatar: Union[str, QPixmap], size: QSize = QSize(45, 45)):
        if isinstance(avatar, str):
            self.setPixmap(QPixmap(avatar).scaled(size))
        elif isinstance(avatar, QPixmap):
            self.setPixmap(avatar.scaled(size))


class MessageDisplay(QLabel):
    """
    """
    def __init__(self, text: str, role: ChatRole, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.role = role

        self.setFont(QFont('微软雅黑', 12))
        self.setWordWrap(True)
        self.setAlignment(Qt.AlignVCenter | Qt.AlignLeft)
        self.setTextInteractionFlags(Qt.TextSelectableByMouse)

        self.setMarkdown(text)

    def paintEvent(self, arg__1: QPaintEvent):
        painter = QPainter(self)
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor('#b2e281') if self.role == ChatRole.User else QColor('white'))
        painter.drawRoundedRect(self.rect(), 6, 6)
        '''
        painter.setPen(QColor('#000000'))
        painter.drawText(self.rect(), self.alignment() | Qt.TextWordWrap, self.text())
        '''
        super().paintEvent(arg__1)

    def setMarkdown(self, text: str):
        self.setTextFormat(Qt.MarkdownText)
        self.setText(text)


class Triangle(QWidget):
    """
    """
    def __init__(self, role: ChatRole, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.role = role

        self.setFixedSize(QSize(6, 45))

    def paintEvent(self, arg__1: QPaintEvent):
        painter = QPainter(self)
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor('#b2e281') if self.role == ChatRole.User else QColor('white'))
        painter.drawPolygon(QPolygon([QPoint(0, 20), QPoint(0, 34), QPoint(6, 27)] if self.role == ChatRole.User else [QPoint(0, 27), QPoint(6, 20), QPoint(6, 34)]))


class MessageLayout(QHBoxLayout):
    """
    """
    def __init__(self, message, role, status, parent = None):
        super().__init__(parent)

        self.setSpacing(0)
        self.setContentsMargins(0, 0, 0, 0)

        self.avatarDisplay = AvatarDisplay(None)
        self.messageDisplay = MessageDisplay(message, role)
        self.triangle = Triangle(role)
        self.status = StatusWidgetBase(status)
        self.spacer = QSpacerItem(self.avatarDisplay.sizeHint().width()+self.triangle.sizeHint().width(), 0, QSizePolicy.MinimumExpanding, QSizePolicy.Minimum)

        if role == ChatRole.User:
            self.addSpacerItem(self.spacer)
            self.addWidget(self.status, 0, Qt.AlignTop) if status is not None else None
            self.addWidget(self.messageDisplay)
            self.addWidget(self.triangle, 0, Qt.AlignTop)
            self.addWidget(self.avatarDisplay, 0, Qt.AlignTop)
        else:
            self.addWidget(self.avatarDisplay, 0, Qt.AlignTop)
            self.addWidget(self.triangle, 0, Qt.AlignTop)
            self.addWidget(self.messageDisplay)
            self.addWidget(self.status, 0, Qt.AlignTop) if status is not None else None
            self.addSpacerItem(self.spacer)


class NoticeDisplay(QLabel):
    """
    """
    def __init__(self, text: str, parent = None):
        super().__init__(text, parent)

        self.setFont(QFont('微软雅黑', 12))
        self.setWordWrap(True)
        self.setAlignment(Qt.AlignCenter)
        self.setTextInteractionFlags(Qt.TextSelectableByMouse)

##############################################################################################################################

class ChatWidgetBase(FrameBase):
    """
    Base class for chatWidget components
    """
    onAvatarClicked = Signal(AvatarDisplay)

    def __init__(self, parent = None):
        super().__init__(parent)

        self.avatarDisplays = {}

        self.scrollArea = VerticalScrollArea()
        self.scrollAreaContent = QWidget()
        self.scrollArea.setWidget(self.scrollAreaContent)
        self.scrollAreaContentLayout = QVBoxLayout(self.scrollAreaContent)
        self.scrollAreaContentLayout.setSpacing(12)
        self.scrollAreaContentLayout.setContentsMargins(12, 12, 12, 12)
        self.scrollAreaContentSpacer = QSpacerItem(0, 0, QSizePolicy.Minimum, QSizePolicy.Expanding)
        self.scrollAreaContentLayout.addSpacerItem(self.scrollAreaContentSpacer)

        layout = QVBoxLayout(self)
        layout.setSpacing(0)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.addWidget(self.scrollArea)

        StyleSheetBase.ChatWidget.apply(self)

    def update(self) -> None:
        self.scrollArea.verticalScrollBar().setValue(self.scrollArea.verticalScrollBar().maximum())
        super().update()

    def _removeallwidgets(self, layout: QLayout, selfIgnored: bool = True):
        for i in range(min(10, layout.count())): # 每次最多处理10个项目
            item = layout.takeAt(0)
            if item is None:
                return
            widget = item.widget()
            if widget is not None:
                widget.deleteLater()
                widget.hide()
            sublayout = item.layout()
            if sublayout is not None:
                self._removeallwidgets(sublayout, selfIgnored = False)
        if layout.count() > 0:
            QTimer.singleShot(0, lambda: self._removeallwidgets(layout, selfIgnored))
        else:
            layout.deleteLater() if not selfIgnored else None

    def clear(self):
        self._removeallwidgets(self.scrollAreaContentLayout)
        self.scrollAreaContentLayout.addSpacerItem(self.scrollAreaContentSpacer)

    def addNotice(self, notice: str):
        self.notice = NoticeDisplay(notice)
        self.scrollAreaContentLayout.insertWidget(self.scrollAreaContentLayout.indexOf(self.scrollAreaContentSpacer), self.notice)
        self.update()

    def _storeAvatar(self, avatarDisplay: AvatarDisplay, role: ChatRole):
        self.avatarDisplays[avatarDisplay] = role
        avatarDisplay.clicked.connect(lambda: self.onAvatarClicked.emit(avatarDisplay))

    def setAvatar(self, avatar, role):
        for avatarDisplay, avatarRole in self.avatarDisplays.items():
            avatarDisplay.setAvatar(avatar) if avatarRole == role else None

    def addMessage(self, message, role, status, stream: bool = False):
        if stream and hasattr(self, "messageLayout") and hasattr(self, 'role') and self.role == role:
            self.messageLayout.messageDisplay.setMarkdown(message)
            self.messageLayout.status.setStatus(status)
            self.update()
            return
        self.role = role
        self.messageLayout = MessageLayout(message, role, status)
        self._storeAvatar(self.messageLayout.avatarDisplay, role)
        self.scrollAreaContentLayout.insertLayout(self.scrollAreaContentLayout.indexOf(self.scrollAreaContentSpacer), self.messageLayout)
        self.update()

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ChatWidget.deregistrate(self)

##############################################################################################################################