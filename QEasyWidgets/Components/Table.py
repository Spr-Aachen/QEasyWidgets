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
    _margin = 3

    def sizeHint(self, option, index):
        size = super().sizeHint(option, index)
        size.setHeight(33)
        size = size.grownBy(QMargins(self._margin, 2 * self._margin, self._margin, 2 * self._margin))
        return size

    def initStyleOption(self, option: QStyleOptionViewItem, index: QModelIndex):
        super().initStyleOption(option, index)
        # get text color
        textBrush = index.data(Qt.ForegroundRole)
        textColor = (Qt.white if isDarkTheme() else Qt.black) if textBrush is None else textBrush.color()
        '''
        # set font
        option.font = index.data(Qt.FontRole)
        '''
        # set text color
        option.palette.setColor(QPalette.Text, textColor)
        option.palette.setColor(QPalette.HighlightedText, textColor)


class TableBase(QTableView):
    """
    Base class for table components
    """
    sorted = Signal()

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.StandardItemModel = QStandardItemModel(self)
        super().setModel(self.StandardItemModel)

        super().setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        super().verticalHeader().setStretchLastSection(False)
        super().verticalHeader().setResizeContentsPrecision(0)
        super().verticalHeader().setSectionResizeMode(QHeaderView.Interactive)
        super().horizontalHeader().setStretchLastSection(False)
        super().horizontalHeader().setResizeContentsPrecision(0)
        super().horizontalHeader().setSectionResizeMode(QHeaderView.Interactive)

        super().setSelectionMode(QAbstractItemView.NoSelection)
        super().setEditTriggers(QAbstractItemView.NoEditTriggers)

        super().verticalHeader().setVisible(False)
        super().horizontalHeader().setVisible(True)
        self.model().insertColumn(0)
        self.model().setHorizontalHeaderItem(0, QStandardItem('Index'))
        super().horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.model().rowsInserted.connect(self.setIndex)
        self.model().rowsRemoved.connect(self.setIndex)
        self.sorted.connect(self.setIndex)

        self.isIndexShown = False
        self.setIndexHeaderVisible(True)

        self.setItemDelegate(ItemDelegate(self))

        StyleSheetBase.Table.apply(self)

    def model(self) -> QStandardItemModel:
        return self.StandardItemModel

    def currentRow(self) -> int:
        return super().currentIndex().row()

    def insertRow(self, row: int) -> None:
        self.model().insertRow(row)

    def removeRow(self, row: int) -> None:
        self.model().removeRow(row)

    def rowCount(self) -> int:
        return self.model().rowCount()

    def setRowCount(self, rows: int) -> None:
        self.model().setRowCount(rows)

    def currentColumn(self) -> int:
        return super().currentIndex().column() + 1

    def insertColumn(self, column: int) -> None:
        self.model().insertColumn(column + 1)

    def removeColumn(self, column: int) -> None:
        self.model().removeColumn(column + 1)

    def columnCount(self) -> int:
        return self.model().columnCount() - 1

    def setColumnCount(self, columns: int) -> None:
        self.model().setColumnCount(columns + 1)

    def setColumnWidth(self, column: int, width: int) -> None:
        super().setColumnWidth(column + 1, width)

    def selectColumn(self, column: int) -> None:
        super().selectColumn(column + 1)

    def insertColumn(self, column: int) -> None:
        self.model().insertColumn(column + 1)

    def sortByColumn(self, column: int, order: Qt.SortOrder) -> None:
        #super().setSortingEnabled(True) if not super().isSortingEnabled() else None
        super().sortByColumn(column + 1, order)
        self.sorted.emit()

    def item(self, row: int, column: int) -> QStandardItem:
        return self.model().item(row, column + 1)

    def setItem(self, row: int, column: int, item: QStandardItem) -> None:
        self.model().setItem(row, column + 1, item)

    def cellWidget(self, row: int, column: int) -> QWidget:
        return super().indexWidget(self.model().index(row, column + 1))

    def setCellWidget(self, row: int, column: int, widget: QWidget) -> None:
        super().setIndexWidget(self.model().index(row, column + 1), widget)

    def setHorizontalHeaderItem(self, column: int, item: QStandardItem) -> None:
        self.model().setHorizontalHeaderItem(column + 1, item)

    def setHorizontalHeaderLabels(self, headers: list[str]) -> None:
        for index, header in enumerate(headers):
            if index == 1 + self.columnCount():
                return print("Maximum headers reached")
            self.setHorizontalHeaderItem(index, QStandardItem(header))

    def horizontalHeaderItem(self, column: int) -> QStandardItem:
        return self.model().horizontalHeaderItem(column)

    def horizontalHeaderLabels(self) -> list[str]:
        return [self.horizontalHeaderItem(column).text() for column in range(self.columnCount())]

    def setIndexHeaderVisible(self, showIndexHeader: bool = True) -> None:
        if showIndexHeader and not self.isIndexShown:
            super().showColumn(0)
            self.isIndexShown = True
        if not showIndexHeader and self.isIndexShown:
            super().hideColumn(0)
            self.isIndexShown = False

    def setIndex(self) -> None:
        for index in range(self.model().rowCount()):
            self.model().setItem(index, 0, QStandardItem(f"{index + 1}"))

    def setSectionVerticalResizeMode(self, row: int, mode: QHeaderView.ResizeMode) -> None:
        super().verticalHeader().setSectionResizeMode(row, mode)

    def setSectionHorizontalResizeMode(self, column: int, mode: QHeaderView.ResizeMode) -> None:
        super().horizontalHeader().setSectionResizeMode(column + 1, mode)

    def selectOuterRow(self, innerWidget: QWidget) -> None:
        cellWidget = innerWidget.parent()
        modelIndex = self.indexAt(cellWidget.pos())
        self.selectRow(modelIndex.row()) #if index.isValid() else None

    def addRow(self, layouts: list[QLayout], resizeModes: list[Optional[QHeaderView.ResizeMode]], columnWidth: list[Optional[int]], height: Optional[int], reverse: bool = False) -> None:
        targetRow = self.rowCount() if not reverse else 0
        columnCount = self.columnCount()
        self.insertRow(targetRow)
        for columnCount in range(columnCount):
            self.setCellWidget(targetRow, columnCount, QWidget())
            self.cellWidget(targetRow, columnCount).setLayout(layouts[columnCount])
            self.setSectionHorizontalResizeMode(columnCount, resizeModes[columnCount]) if resizeModes[columnCount] is not None else None
            self.setColumnWidth(columnCount, columnWidth[columnCount]) if columnWidth[columnCount] is not None else None
        self.setRowHeight(targetRow, height) if height is not None else None

    def delRow(self) -> None:
        self.removeRow(self.currentRow()) if self.rowCount() > 1 else None

    def clearRows(self):
        while self.rowCount() > 0:
            self.removeRow(0)

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Table.deregistrate(self)

##############################################################################################################################