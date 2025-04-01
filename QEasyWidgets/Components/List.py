from typing import Optional, overload
from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *

from ..Common.StyleSheet import *
from ..Common.QFunctions import *
from .Menu import MenuBase

##############################################################################################################################

class ListBase(QListView):
    """
    Base class for list components
    """
    _contextMenu = None

    currentItemChanged = Signal(QStandardItem, QStandardItem)
    itemClicked = Signal(QStandardItem)

    def __init__(self, parent: Optional[QWidget] = None):
        super().__init__(parent)

        self.StandardItemModel = QStandardItemModel(self)
        self.setModel(self.StandardItemModel)

        self.clicked.connect(self.onItemClicked)

        setFont(self, 12)

        StyleSheetBase.List.apply(self)

    def model(self) -> QStandardItemModel:
        return self.StandardItemModel

    def _toStandardItem(self, item):
        if isinstance(item, QListWidgetItem):
            item = QStandardItem(item.text())
        if isinstance(item, str):
            item = QStandardItem(item)
        return item

    def row(self, item):
        item = self._toStandardItem(item)
        index = self.model().indexFromItem(item)
        return index.row()

    def count(self):
        return self.model().rowCount()

    def item(self, row):
        return self.model().item(row)

    def addItem(self, item):
        item = self._toStandardItem(item)
        self.model().appendRow(item)

    def takeItem(self, row):
        return self.model().takeRow(row)[0] # takeRow returns a list

    def currentItem(self):
        indexes = self.selectedIndexes()
        if indexes:
            return self.model().itemFromIndex(indexes[0])
        return None

    def setCurrentItem(self, item):
        item = self._toStandardItem(item)
        index = self.model().indexFromItem(item)
        self.setCurrentIndex(index)

    def clear(self):
        self.model().clear()

    def click(self, item):
        item = self._toStandardItem(item)
        index = self.model().indexFromItem(item)
        self.setCurrentIndex(index)
        self.clicked.emit(index)

    @Slot('QModelIndex')
    def onItemClicked(self, index: Union[QModelIndex, QPersistentModelIndex]):
        item = self.model().itemFromIndex(index)
        self.itemClicked.emit(item)

    def currentChanged(self, current: Union[QModelIndex, QPersistentModelIndex], previous: Union[QModelIndex, QPersistentModelIndex]):
        super().currentChanged(current, previous)
        currentItem, previousItem = self.model().itemFromIndex(current), self.model().itemFromIndex(previous)
        self.currentItemChanged.emit(currentItem, previousItem)

    @property
    def contextMenu(self):
        if self._contextMenu is None:
            self._contextMenu = MenuBase(self)
        return self._contextMenu

    @contextMenu.setter
    def contextMenu(self, menu: MenuBase):
        ''''''
        self._contextMenu = menu

    def setContextMenu(self, actions: dict)-> None:
        self.setContextMenuPolicy(Qt.CustomContextMenu) if self.contextMenuPolicy() != Qt.CustomContextMenu else None
        self.customContextMenuRequested.connect(
            lambda position: (
                self.contextMenu.clear(),
                showContextMenu(self, self.contextMenu, actions, self.mapToGlobal(position))
            )
        )

    def setBorderless(self, borderless: bool) -> None:
        self.setProperty("isBorderless", borderless)

    def clearDefaultStyleSheet(self) -> None:
        StyleSheetBase.List.deregistrate(self)

##############################################################################################################################