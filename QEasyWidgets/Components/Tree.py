from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.Theme import *
from ..Common.StyleSheet import *
from ..Common.QFunctions import *

##############################################################################################################################

class ItemDelegate(QStyledItemDelegate):
    '''
    '''
    def initStyleOption(self, option: QStyleOptionViewItem, index: QModelIndex):
        super().initStyleOption(option, index)
        # get text color
        textBrush = index.data(Qt.ForegroundRole)
        textColor = (Qt.white if EasyTheme.THEME == Theme.Dark else Qt.black) if textBrush is None else textBrush.color()
        '''
        # set font
        option.font = index.data(Qt.FontRole)
        '''
        # set text color
        option.palette.setColor(QPalette.Text, textColor)
        option.palette.setColor(QPalette.HighlightedText, textColor)


class TreeWidgetBase(QTreeWidget):
    """
    Base class for treeWidget components
    """
    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.setColumnCount(1)

        self.header().setHighlightSections(False)
        self.header().setDefaultAlignment(Qt.AlignCenter)
        #self.setHeaderHidden(True)

        self.setItemDelegate(ItemDelegate(self))
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

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Tree.Deregistrate(self)

##############################################################################################################################