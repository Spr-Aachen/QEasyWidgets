from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.QFunctions import *
from ..Resources.Sources import *

##############################################################################################################################
"""
class ItemDelegate(QStyledItemDelegate):
    '''
    '''
    def __init__(self, parent: QTreeView):
        super().__init__(parent)

    def _drawCheckBox(self, painter: QPainter, option: QStyleOptionViewItem, index: QModelIndex):
        painter.save()
        checkState = Qt.CheckState(index.data(Qt.ItemDataRole.CheckStateRole))

        isDark = isDarkTheme()

        r = 4.5
        x = option.rect.x() + 23
        y = option.rect.center().y() - 9
        rect = QRectF(x, y, 19, 19)

        if checkState == Qt.CheckState.Unchecked:
            painter.setBrush(QColor(0, 0, 0, 26)
                             if isDark else QColor(0, 0, 0, 6))
            painter.setPen(QColor(255, 255, 255, 142)
                           if isDark else QColor(0, 0, 0, 122))
            painter.drawRoundedRect(rect, r, r)
        else:
            painter.setPen(themeColor())
            painter.setBrush(themeColor())
            painter.drawRoundedRect(rect, r, r)

            if checkState == Qt.CheckState.Checked:
                CheckBoxIcon.ACCEPT.render(painter, rect)
            else:
                CheckBoxIcon.PARTIAL_ACCEPT.render(painter, rect)

        painter.restore()

    def paint(self, painter: QPainter, option: QStyleOptionViewItem, index: Union[QModelIndex, QPersistentModelIndex]) -> None:
        painter.setRenderHints(QPainter.Antialiasing | QPainter.TextAntialiasing)
        super().paint(painter, option, index)

        if index.data(Qt.CheckStateRole) is not None:
            self._drawCheckBox(painter, option, index)

        if not (option.state & (QStyle.State_Selected | QStyle.State_MouseOver)):
            return

        painter.save()
        painter.setPen(Qt.NoPen)

        # draw background
        h = option.rect.height() - 4
        brush = QColor(255, 255, 255, 9) if EasyTheme.THEME == Theme.Light else QColor(0, 0, 0, 0)
        painter.setBrush(brush)
        painter.drawRoundedRect(4, option.rect.y() + 2, self.parent().width() - 8, h, 4, 4)

        # draw indicator
        if option.state & QStyle.State_Selected and self.parent().horizontalScrollBar().value() == 0:
            painter.setBrush(themeColor())
            painter.drawRoundedRect(4, 9+option.rect.y(), 3, h - 13, 1.5, 1.5)

        painter.restore()

    def initStyleOption(self, option, index):
        super().initStyleOption(option, index)

        # font
        option.font = index.data(Qt.FontRole) or getFont(13)

        # text color
        textColor = Qt.white if isDarkTheme() else Qt.black
        textBrush = index.data(Qt.ForegroundRole)
        if textBrush is not None:
            textColor = textBrush.color()

        option.palette.setColor(QPalette.Text, textColor)
        option.palette.setColor(QPalette.HighlightedText, textColor)
"""

class TreeWidgetBase(QTreeWidget):
    '''
    '''
    def __init__(self, parent=None):
        super().__init__(parent=parent)

        self.setColumnCount(1)

        self.header().setHighlightSections(False)
        self.header().setDefaultAlignment(Qt.AlignCenter)
        #self.setHeaderHidden(True)

        self.setItemDelegate(QStyledItemDelegate(self))
        self.setIconSize(QSize(16, 16))

        StyleSheetBase.Tree.Apply(self)

    def drawBranches(self, painter: QPainter, rect: QRect, index: Union[QModelIndex, QPersistentModelIndex]) -> None:
        #rect.moveLeft(3)
        super().drawBranches(painter, rect, index)

    def rootItems(self) -> list[QTreeWidgetItem]:
        RootItems = [self.topLevelItem(Index) for Index in range(0, self.topLevelItemCount())]
        return RootItems

    def rootItemTexts(self) -> list[str]:
        RootItemTexts = [RootItem.text(0) for RootItem in self.rootItems()]
        return RootItemTexts

    def childItems(self, RootItem: QTreeWidgetItem) -> list[QTreeWidgetItem]:
        ChildItems = [RootItem.child(Index) for Index in range(0, RootItem.childCount())]
        return ChildItems

    def childItemTexts(self, RootItem: QTreeWidgetItem) -> list[str]:
        ChildItemTexts = [ChildItem.text(0) for ChildItem in self.childItems(RootItem)]
        return ChildItemTexts

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Tree.Deregistrate(self)

##############################################################################################################################