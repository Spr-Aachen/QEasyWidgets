from PySide6.QtCore import Qt, QSize, QPoint, QTimer
from PySide6.QtGui import QPainter, QFont, QColor, QPixmap, QPolygon, QPaintEvent
from PySide6.QtWidgets import QWidget, QFrame, QLabel, QLayout, QHBoxLayout, QSizePolicy, QVBoxLayout, QSpacerItem

from ..Common.StyleSheet import *
from .StatusWidget import StatusWidgetBase
from .ScrollArea import VerticalScrollArea

##############################################################################################################################

class Notice(QLabel):
    def __init__(self, text: str, parent = None):
        super().__init__(text, parent)

        self.setFont(QFont('微软雅黑', 12))
        self.setWordWrap(True)
        self.setAlignment(Qt.AlignCenter)
        self.setTextInteractionFlags(Qt.TextSelectableByMouse)

##############################################################################################################################

class Message(QLabel):
    def __init__(self, text: str, isSent: bool = False, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.isSent = isSent

        self.setFont(QFont('微软雅黑', 12))
        self.setWordWrap(True)
        self.setAlignment(Qt.AlignVCenter | Qt.AlignLeft)
        self.setTextInteractionFlags(Qt.TextSelectableByMouse)

        self.setMarkdown(text)

    def paintEvent(self, arg__1: QPaintEvent):
        painter = QPainter(self)
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor('#b2e281') if self.isSent else QColor('white'))
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
    def __init__(self, isSent: bool = False, size: QSize = QSize(6, 45), parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.isSent = isSent

        self.setFixedSize(size)

    def paintEvent(self, arg__1: QPaintEvent) -> None:
        painter = QPainter(self)
        painter.setPen(Qt.NoPen)
        painter.setBrush(QColor('#b2e281') if self.isSent else QColor('white'))
        triangle = QPolygon([QPoint(0, 20), QPoint(0, 34), QPoint(6, 27)] if self.isSent else [QPoint(0, 27), QPoint(6, 20), QPoint(6, 34)])
        painter.drawPolygon(triangle)


class Avatar(QLabel):
    def __init__(self, avatar: Union[str, QPixmap] = None, size: QSize = QSize(45, 45), parent: Optional[QWidget] = None):
        super().__init__(parent)

        if isinstance(avatar, str):
            self.setPixmap(QPixmap(avatar).scaled(size))
        elif isinstance(avatar, QPixmap):
            self.setPixmap(avatar.scaled(size))

        self.setFixedSize(size)


class MessageLayout(QHBoxLayout):
    def __init__(self, str_content, isSent, status, parent = None):
        super().__init__(parent)

        self.setSpacing(0)
        self.setContentsMargins(0, 0, 0, 0)

        self.avatar = Avatar(None)
        self.triangle = Triangle(isSent)
        self.message = Message(str_content, isSent)
        self.status = StatusWidgetBase(status)
        self.spacer = QSpacerItem(self.avatar.sizeHint().width()+self.triangle.sizeHint().width(), 0, QSizePolicy.MinimumExpanding, QSizePolicy.Minimum)

        if isSent:
            self.addSpacerItem(self.spacer)
            self.addWidget(self.status, 0, Qt.AlignTop) if status is not None else None
            self.addWidget(self.message)
            self.addWidget(self.triangle, 0, Qt.AlignTop)
            self.addWidget(self.avatar, 0, Qt.AlignTop)
        else:
            self.addWidget(self.avatar, 0, Qt.AlignTop)
            self.addWidget(self.triangle, 0, Qt.AlignTop)
            self.addWidget(self.message)
            self.addWidget(self.status, 0, Qt.AlignTop) if status is not None else None
            self.addSpacerItem(self.spacer)

##############################################################################################################################

class ChatWidgetBase(QFrame):
    def __init__(self, parent = None):
        super().__init__(parent)

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

        StyleSheetBase.ChatWidget.Apply(self)

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

    def addNotice(self, str_content):
        self.notice = Notice(str_content)
        self.scrollAreaContentLayout.insertWidget(self.scrollAreaContentLayout.indexOf(self.scrollAreaContentSpacer), self.notice)
        self.update()

    def addMessage(self, str_content, isSent, status, stream = False):
        if stream and hasattr(self, "messageLayout") and hasattr(self, 'role') and self.role == isSent:
            self.messageLayout.message.setMarkdown(str_content)
            self.messageLayout.status.setStatus(status)
            self.update()
            return
        self.role = isSent
        self.messageLayout = MessageLayout(str_content, isSent, status)
        self.scrollAreaContentLayout.insertLayout(self.scrollAreaContentLayout.indexOf(self.scrollAreaContentSpacer), self.messageLayout)
        self.update()

    def setMessages(self, messages):
        self.clear()
        for message in messages:
            self.addMessage(*message, False)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ChatWidget.Deregistrate(self)

##############################################################################################################################