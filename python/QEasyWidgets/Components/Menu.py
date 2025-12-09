from typing import List, Union, Optional, overload
from PyEasyUtils import singledispatchmethod
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

from ..Common.Config import Direction
from ..Common.Icon import *
from ..Common.Theme import *
from ..Common.StyleSheet import StyleSheetBase
from .ScrollArea import ScrollDelegate
from ..Common.QFunctions import getCurrentScreen, getScreenGeometry

##############################################################################################################################

font = QFont()
font.setPixelSize(12)

##############################################################################################################################

class MenuItemDelegate(QStyledItemDelegate):
    """
    Menu item delegate
    """
    def _isSeparator(self, index: QModelIndex):
        return index.model().data(index, Qt.DecorationRole) == "seperator"

    def paint(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex):
        if not self._isSeparator(index):
            return super().paint(painter, option, index)
        # draw seperator
        painter.save()
        c = 0 if not isDarkTheme() else 255
        pen = QPen(QColor(c, c, c, 25), 1)
        pen.setCosmetic(True)
        painter.setPen(pen)
        rect = option.rect
        painter.drawLine(0, rect.y() + 4, rect.width() + 12, rect.y() + 4)
        painter.restore()
        # draw shortcut key
        action: QAction = index.data(Qt.UserRole)
        if not isinstance(action, QAction) or action.shortcut().isEmpty():
            return
        painter.save()
        if not option.state & QStyle.State_Enabled:
            painter.setOpacity(0.5 if isDarkTheme() else 0.6)
        painter.setPen(QColor(255, 255, 255, 200) if isDarkTheme() else QColor(0, 0, 0, 153))
        fm = QFontMetrics(font)
        shortcut = action.shortcut().toString(QKeySequence.NativeText)
        sw = fm.boundingRect(shortcut).width()
        painter.translate(option.rect.width()-sw-20, 0)
        rect = QRectF(0, option.rect.y(), sw, option.rect.height())
        painter.drawText(rect, Qt.AlignLeft | Qt.AlignVCenter, shortcut)
        painter.restore()

##############################################################################################################################

class MenuAnimationManager(QObject):
    """
    Menu animation manager
    """
    managers = {}

    def __init__(self, menu: QMenu):
        super().__init__()

        self.menu = menu
        self.anim = QPropertyAnimation(menu, b'pos', menu)

        self.anim.setDuration(250)
        self.anim.setEasingCurve(QEasingCurve.OutQuad)
        self.anim.valueChanged.connect(self._onValueChanged)
        self.anim.valueChanged.connect(self._updateMenuViewport)

    def _menuSize(self):
        m = self.menu.layout().contentsMargins()
        w = self.menu.view.width() + m.left() + m.right() + 120
        h = self.menu.view.height() + m.top() + m.bottom() + 20
        return w, h

    def _onValueChanged(self):
        pass

    def _updateMenuViewport(self):
        self.menu.view.viewport().update()
        self.menu.view.setAttribute(Qt.WA_UnderMouse, True)
        e = QHoverEvent(QEvent.HoverEnter, QPoint(), QPoint(1, 1))
        QApplication.sendEvent(self.menu.view, e)

    def availableViewSize(self, pos: QPoint):
        ss = getScreenGeometry(getCurrentScreen())
        w, h = ss.width() - 100, ss.height() - 100
        return w, h

    def exec(self, pos: QPoint):
        rect = getScreenGeometry(getCurrentScreen())
        x = min(pos.x() - self.menu.layout().contentsMargins().left(), rect.right() - (self.menu.width() + 5))
        y = min(pos.y() - 4, rect.bottom() - (self.menu.height()) + 10)
        self.menu.move(QPoint(x, y))

    @classmethod
    def register(cls, name):
        def wrapper(manager):
            if name not in cls.managers:
                cls.managers[name] = manager
            return manager
        return wrapper

    @classmethod
    def make(cls, menu: QMenu, animType: Direction):
        return cls.managers[animType](menu) if animType in cls.managers else cls(menu)


@MenuAnimationManager.register(Direction.Down)
class DropDownMenuAnimationManager(MenuAnimationManager):
    """
    Drop down menu animation manager
    """
    def _onValueChanged(self):
        y = self.anim.endValue().y() - self.anim.currentValue().y()
        w, h = self._menuSize()
        self.menu.setMask(QRegion(0, y, w, h))

    def availableViewSize(self, pos: QPoint):
        ss = getScreenGeometry(getCurrentScreen())
        return ss.width() - 100, max(ss.bottom() - pos.y() - 10, 1)

    def exec(self, pos):
        rect = getScreenGeometry(getCurrentScreen())
        x = min(pos.x() - self.menu.layout().contentsMargins().left(), rect.right() - (self.menu.width() + 5))
        y = min(pos.y() - 4, rect.bottom() - (self.menu.height()) + 10)
        pos = QPoint(x, y)
        h = self.menu.height() + 5

        self.anim.setStartValue(pos - QPoint(0, int(h/2)))
        self.anim.setEndValue(pos)
        self.anim.start()


@MenuAnimationManager.register(Direction.Up)
class PullUpMenuAnimationManager(MenuAnimationManager):
    """
    Pull up menu animation manager
    """
    def _onValueChanged(self):
        y = self.anim.endValue().y() - self.anim.currentValue().y()
        w, h = self._menuSize()
        self.menu.setMask(QRegion(0, y, w, h - 28))

    def availableViewSize(self, pos: QPoint):
        ss = getScreenGeometry(getCurrentScreen())
        return ss.width() - 100, max(pos.y() - ss.top() - 28, 1)

    def exec(self, pos):
        rect = getScreenGeometry(getCurrentScreen())
        x = min(pos.x() - self.menu.layout().contentsMargins().left(), rect.right() - (self.menu.width() + 5))
        y = max(pos.y() - (self.menu.height()) + 10, rect.top() + 4)
        pos = QPoint(x, y)
        h = self.menu.height() + 5

        self.anim.setStartValue(pos + QPoint(0, int(h/2)))
        self.anim.setEndValue(pos)
        self.anim.start()

##############################################################################################################################

class MenuActionListWidget(QListWidget):
    """
    Menu action list widget
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self._itemHeight = 28
        self._maxVisibleItems = -1  # adjust visible items according to the size of screen

        self.setViewportMargins(0, 6, 0, 6)
        self.setTextElideMode(Qt.ElideNone)
        self.setDragEnabled(False)
        self.setMouseTracking(True)
        self.setVerticalScrollMode(self.ScrollMode.ScrollPerPixel)
        self.setIconSize(QSize(14, 14))
        self.setItemDelegate(MenuItemDelegate(self))

        self.scrollDelegate = ScrollDelegate(self)
        self.setStyleSheet('MenuActionListWidget{font: 14px "Segoe UI", "Microsoft YaHei", "PingFang SC"}')

        self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

    def adjustSize(self, pos = None, animType = None):
        size = QSize()
        for i in range(self.count()):
            s = self.item(i).sizeHint()
            size.setWidth(max(s.width(), size.width(), 1))
            size.setHeight(max(1, size.height() + s.height()))
        # adjust the height of viewport
        w, h = MenuAnimationManager.make(self, animType).availableViewSize(pos)
        self.viewport().adjustSize()
        # adjust the height of list widget
        m = self.viewportMargins()
        size += QSize(m.left()+m.right()+2, m.top()+m.bottom())
        size.setHeight(min(h, size.height()+3))
        size.setWidth(max(min(w, size.width()), self.minimumWidth()))
        if self.maxVisibleItems() > 0:
            size.setHeight(min(
                size.height(), self.maxVisibleItems() * self._itemHeight + m.top()+m.bottom() + 3))
        self.setFixedSize(size)

    def insertItem(self, row, item):
        super().insertItem(row, item)
        self.adjustSize()

    def addItem(self, item):
        super().addItem(item)
        self.adjustSize()

    def takeItem(self, row):
        item = super().takeItem(row)
        self.adjustSize()
        return item

    def maxVisibleItems(self):
        return self._maxVisibleItems

    def setMaxVisibleItems(self, num: int):
        self._maxVisibleItems = num
        self.adjustSize()

    def setItemHeight(self, height: int):
        if height == self._itemHeight:
            return
        for i in range(self.count()):
            item = self.item(i)
            if not self.itemWidget(item):
                item.setSizeHint(QSize(item.sizeHint().width(), height))
        self._itemHeight = height
        self.adjustSize()

    def itemsHeight(self):
        N = self.count() if self.maxVisibleItems() < 0 else min(self.maxVisibleItems(), self.count())
        h = sum(self.item(i).sizeHint().height() for i in range(N))
        m = self.viewportMargins()
        return h + m.top() + m.bottom()

    def heightForAnimation(self, pos: QPoint, animType: Direction):
        ih = self.itemsHeight()
        _, sh = MenuAnimationManager.make(self, animType).availableViewSize(pos)
        return min(ih, sh)


class SubMenuItemWidget(QWidget):
    """
    Sub menu item
    """
    showMenuSig = Signal(QListWidgetItem)

    def __init__(self, menu: QMenu, item: QListWidgetItem, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.menu = menu
        self.item = item

    def enterEvent(self, e: QEnterEvent):
        super().enterEvent(e)
        self.showMenuSig.emit(self.item)

    def paintEvent(self, e: QPaintEvent):
        painter = QPainter(self)
        painter.setRenderHints(QPainter.Antialiasing)
        # draw right arrow
        IconBase.Chevron_Right.paint(
            painter,
            QRectF(self.width()-10, self.height()/2-9/2, 9, 9)
        )

##############################################################################################################################

class MenuBase(QMenu):
    """
    Round corner menu
    """
    closedSignal = Signal()

    @singledispatchmethod
    def __init__(self, title: str = "", parent: Optional[QWidget] = None):
        super().__init__(parent)

        self._title = title
        self._icon = QIcon()
        self._actions: List[QAction] = []
        self._subMenus = []

        self.isSubMenu = False
        self.parentMenu = None
        self.menuItem = None
        self.lastHoverItem = None
        self.lastHoverSubMenuItem = None
        self.isHideBySystem = True
        self.itemHeight = 28

        self.hBoxLayout = QHBoxLayout(self)
        self.view = MenuActionListWidget(self)

        self.animManager = None
        self.timer = QTimer(self)

        self.setWindowFlags(Qt.Popup | Qt.FramelessWindowHint | Qt.NoDropShadowWindowHint)
        self.setAttribute(Qt.WA_TranslucentBackground)
        self.setMouseTracking(True)

        self.timer.setSingleShot(True)
        self.timer.setInterval(400)
        self.timer.timeout.connect(self._onShowMenuTimeOut)

        self.setShadowEffect()
        self.hBoxLayout.addWidget(self.view, 1, Qt.AlignCenter)

        self.hBoxLayout.setContentsMargins(12, 8, 12, 20)

        self.view.itemClicked.connect(self._onItemClicked)
        self.view.itemEntered.connect(self._onItemEntered)

        StyleSheetBase.Menu.apply(self)
    
    @__init__.register
    def _(self, parent: Optional[QWidget] = None):
        self.__init__(parent)
        self._title = ""

    def setMaxVisibleItems(self, num: int):
        self.view.setMaxVisibleItems(num)
        self.adjustSize()

    def setItemHeight(self, height):
        if height == self.itemHeight:
            return
        self.itemHeight = height
        self.view.setItemHeight(height)

    def setShadowEffect(self, blurRadius=30, offset=(0, 8), color=QColor(0, 0, 0, 30)):
        self.shadowEffect = QGraphicsDropShadowEffect(self.view)
        self.shadowEffect.setBlurRadius(blurRadius)
        self.shadowEffect.setOffset(*offset)
        self.shadowEffect.setColor(color)
        self.view.setGraphicsEffect(None)
        self.view.setGraphicsEffect(self.shadowEffect)

    def _setParentMenu(self, parent, item):
        self.parentMenu = parent
        self.menuItem = item
        self.isSubMenu = True if parent else False

    def adjustSize(self):
        m = self.layout().contentsMargins()
        w = self.view.width() + m.left() + m.right()
        h = self.view.height() + m.top() + m.bottom()
        self.setFixedSize(w, h)

    def title(self):
        return self._title

    def icon(self):
        return self._icon

    def setIcon(self, icon: Union[QIcon, IconBase]):
        self._icon = Function_ToQIcon(icon)

    def _hasItemIcon(self):
        return any(not i.icon().isNull() for i in self._actions+self._subMenus)

    def _createItemIcon(self, item):
        hasIcon = self._hasItemIcon()
        icon = QIcon(IconEngine(item.icon()))
        if hasIcon and item.icon().isNull():
            pixmap = QPixmap(self.view.iconSize())
            pixmap.fill(Qt.transparent)
            icon = QIcon(pixmap)
        elif not hasIcon:
            icon = QIcon()
        return icon

    def _longestShortcutWidth(self):
        fm = QFontMetrics(font)
        return max(fm.boundingRect(a.shortcut().toString()).width() for a in self.menuActions())

    def _adjustItemText(self, item: QListWidgetItem, action: QAction):
        # leave some space for shortcut key
        if isinstance(self.view.itemDelegate(), MenuItemDelegate):
            sw = self._longestShortcutWidth()
            if sw:
                sw += 22
        else:
            sw = 0
        # adjust the width of item
        if not self._hasItemIcon():
            item.setText(action.text())
            w = 40 + self.view.fontMetrics().boundingRect(action.text()).width() + sw
        else:
            # add a blank character to increase space between icon and text
            item.setText(" " + action.text())
            space = 4 - self.view.fontMetrics().boundingRect(" ").width()
            w = 60 + self.view.fontMetrics().boundingRect(item.text()).width() + sw + space
        item.setSizeHint(QSize(w, self.itemHeight))
        return w

    def _onActionChanged(self):
        action: QAction = self.sender()
        item: QListWidgetItem = action.property('item')
        item.setIcon(self._createItemIcon(action))
        self._adjustItemText(item, action)
        if action.isEnabled():
            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
        else:
            item.setFlags(Qt.NoItemFlags)
        self.view.adjustSize()
        self.adjustSize()

    def _createActionItem(self, action: QAction, before=None):
        if not before:
            self._actions.append(action)
            super().addAction(action)
        elif before in self._actions:
            index = self._actions.index(before)
            self._actions.insert(index, action)
            super().insertAction(before, action)
        else:
            raise ValueError('`before` is not in the action list')
        item = QListWidgetItem(self._createItemIcon(action), action.text())
        self._adjustItemText(item, action)
        if not action.isEnabled():
            # disable item
            item.setFlags(Qt.NoItemFlags)
        item.setData(Qt.UserRole, action)
        action.setProperty('item', item)
        action.changed.connect(self._onActionChanged)
        return item

    def menuActions(self):
        return self._actions

    def addAction(self, action: QAction):
        item = self._createActionItem(action)
        self.view.addItem(item)
        self.adjustSize()

    def addActions(self, actions: List[QAction]):
        for action in actions:
            self.addAction(action)

    def setDefaultAction(self, action: QAction):
        if action not in self._actions:
            return
        item = action.property("item")
        if item:
            self.view.setCurrentItem(item)

    def insertAction(self, before: QAction, action: QAction):
        if before not in self._actions:
            return
        beforeItem = before.property('item')
        if not beforeItem:
            return
        index = self.view.row(beforeItem)
        item = self._createActionItem(action, before)
        self.view.insertItem(index, item)
        self.adjustSize()

    def insertActions(self, before: QAction, actions: List[QAction]):
        for action in actions:
            self.insertAction(before, action)

    def removeAction(self, action: QAction):
        if action not in self._actions:
            return
        # remove action
        item = action.property("item")
        self._actions.remove(action)
        action.setProperty('item', None)
        if not item:
            return
        # remove item
        self.view.takeItem(self.view.row(item))
        item.setData(Qt.UserRole, None)
        super().removeAction(action)
        # delete widget
        widget = self.view.itemWidget(item)
        if widget:
            widget.deleteLater()

    def clear(self):
        for i in range(len(self._actions)-1, -1, -1):
            self.removeAction(self._actions[i])

    def addWidget(self, widget: QWidget, selectable=True, onClick=None):
        action = QAction()
        action.setProperty('selectable', selectable)
        item = self._createActionItem(action)
        item.setSizeHint(widget.size())
        self.view.addItem(item)
        self.view.setItemWidget(item, widget)
        if not selectable:
            item.setFlags(Qt.NoItemFlags)
        if onClick:
            action.triggered.connect(onClick)
        self.adjustSize()

    def _showSubMenu(self, item):
        self.lastHoverItem = item
        self.lastHoverSubMenuItem = item
        # delay 400 ms to anti-shake
        self.timer.stop()
        self.timer.start()

    def _createSubMenuItem(self, menu):
        self._subMenus.append(menu)
        item = QListWidgetItem(self._createItemIcon(menu), menu.title())
        if not self._hasItemIcon():
            w = 60 + self.view.fontMetrics().boundingRect(menu.title()).width()
        else:
            # add a blank character to increase space between icon and text
            item.setText(" " + item.text())
            w = 72 + self.view.fontMetrics().boundingRect(item.text()).width()
        # add submenu item
        menu._setParentMenu(self, item)
        item.setSizeHint(QSize(w, self.itemHeight))
        item.setData(Qt.UserRole, menu)
        w = SubMenuItemWidget(menu, item, self)
        w.showMenuSig.connect(self._showSubMenu)
        w.resize(item.sizeHint())
        return item, w

    def addMenu(self, menu):
        if not isinstance(menu, MenuBase):
            raise ValueError('`menu` should be an instance of `MenuBase`.')
        item, w = self._createSubMenuItem(menu)
        self.view.addItem(item)
        self.view.setItemWidget(item, w)
        self.adjustSize()

    def insertMenu(self, before: QAction, menu):
        if not isinstance(menu, MenuBase):
            raise ValueError('`menu` should be an instance of `MenuBase`.')
        if before not in self._actions:
            raise ValueError('`before` should be in menu action list')
        item, w = self._createSubMenuItem(menu)
        self.view.insertItem(self.view.row(before.property('item')), item)
        self.view.setItemWidget(item, w)
        self.adjustSize()

    def _onShowMenuTimeOut(self):
        if self.lastHoverSubMenuItem is None or not self.lastHoverItem is self.lastHoverSubMenuItem:
            return
        w = self.view.itemWidget(self.lastHoverSubMenuItem)
        if w.menu.parentMenu.isHidden():
            return
        pos = w.mapToGlobal(QPoint(w.width()+5, -5))
        w.menu.exec(pos)

    def addSeparator(self):
        m = self.view.viewportMargins()
        w = self.view.width()-m.left()-m.right()
        # add separator to list widget
        item = QListWidgetItem()
        item.setFlags(Qt.NoItemFlags)
        item.setSizeHint(QSize(w, 9))
        self.view.addItem(item)
        item.setData(Qt.DecorationRole, "seperator")
        self.adjustSize()

    def closeEvent(self, e: QCloseEvent):
        e.accept()
        self.closedSignal.emit()
        self.view.clearSelection()

    def _hideMenu(self, isHideBySystem=False):
        self.isHideBySystem = isHideBySystem
        self.view.clearSelection()
        if self.isSubMenu:
            self.hide()
        else:
            self.close()

    def _closeParentMenu(self):
        menu = self
        while menu:
            menu.close()
            menu = menu.parentMenu

    def hideEvent(self, e: QHideEvent):
        if self.isHideBySystem and self.isSubMenu:
            self._closeParentMenu()
        self.isHideBySystem = True
        e.accept()

    def mousePressEvent(self, e: QMouseEvent):
        w = self.childAt(e.pos())
        if (w is not self.view) and (not self.view.isAncestorOf(w)):
            self._hideMenu(True)

    def mouseMoveEvent(self, e: QMouseEvent):
        if not self.isSubMenu:
            return
        # hide submenu when mouse moves out of submenu item
        pos = e.globalPos()
        view = self.parentMenu.view
        # get the rect of menu item
        margin = view.viewportMargins()
        rect = view.visualItemRect(self.menuItem).translated(view.mapToGlobal(QPoint()))
        rect = rect.translated(margin.left(), margin.top()+2)
        if self.parentMenu.geometry().contains(pos) and not rect.contains(pos) and \
                not self.geometry().contains(pos):
            view.clearSelection()
            self._hideMenu(False)

    def _onItemClicked(self, item):
        action: QAction = item.data(Qt.UserRole)
        if action not in self._actions or not action.isEnabled():
            return
        if self.view.itemWidget(item) and not action.property('selectable'):
            return
        self._hideMenu(False)
        if not self.isSubMenu:
            action.trigger()
            return
        # close parent menu
        self._closeParentMenu()
        action.trigger()

    def _onItemEntered(self, item):
        self.lastHoverItem = item
        if not isinstance(item.data(Qt.UserRole), MenuBase):
            return
        self._showSubMenu(item)

    def adjustPosition(self):
        m = self.layout().contentsMargins()
        rect = getScreenGeometry(getCurrentScreen())
        w, h = self.layout().sizeHint().width() + 5, self.layout().sizeHint().height()
        x = min(self.x() - m.left(), rect.right() - w)
        y = self.y()
        if y > rect.bottom() - h:
            y = self.y() - h + m.bottom()
        self.move(x, y)

    def paintEvent(self, e: QPaintEvent):
        pass

    def exec(self, pos, animType = Direction.Down):
        self.animManager = MenuAnimationManager.make(self, animType)
        self.animManager.exec(pos)
        self.show()
        if self.isSubMenu:
            self.menuItem.setSelected(True)

    def exec_(self, pos: QPoint, animType=Direction.Down):
        self.exec(pos, animType)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Menu.deregistrate(self)

##############################################################################################################################