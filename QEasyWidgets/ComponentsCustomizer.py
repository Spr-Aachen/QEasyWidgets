from PySide6.QtGui import *
from PySide6.QtCore import *
from PySide6.QtWidgets import *
from PySide6.QtMultimedia import QMediaPlayer, QAudioOutput

from .QFunctions import *
from .Sources import *

##############################################################################################################################

class WidgetBase(QWidget):
    '''
    '''
    Resized = Signal()

    def __init__(self,
        parent: Optional[QWidget] = None,
    ):
        super().__init__(parent)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.Resized.emit()
        '''
        if self.minimumSizeHint() != self.size():
            self.adjustSize()
        '''
        super().resizeEvent(event)

##############################################################################################################################

class ButtonBase(QPushButton):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
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

    def _drawIcon(self, icon, painter, rect):
        Function_DrawIcon(icon, painter, rect)

    def paintEvent(self, e: QPaintEvent) -> None:
        super().paintEvent(e)
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
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
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

##############################################################################################################################

class SpinBoxBase(QSpinBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.Deregistrate(self)


class DoubleSpinBoxBase(QDoubleSpinBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.SpinBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.SpinBox.Deregistrate(self)

##############################################################################################################################

class ComboBoxBase(QComboBox):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.setFocusPolicy(Qt.StrongFocus)

        StyleSheetBase.ComboBox.Apply(self)

    def wheelEvent(self, event: QWheelEvent) -> None:
        event.ignore()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ComboBox.Deregistrate(self)

##############################################################################################################################

class ScrollAreaBase(QScrollArea):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        StyleSheetBase.ScrollArea.Apply(self)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ScrollArea.Deregistrate(self)

##############################################################################################################################

class LabelBase(QLabel):
    '''
    '''
    Resized = Signal()

    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        StyleSheetBase.Label.Apply(self)

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.Resized.emit()
        super().resizeEvent(event)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Label.Deregistrate(self)

##############################################################################################################################

class ToolPage(WidgetBase):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None,
    ):
        super().__init__(parent)

        self.IsExpanded = True

        self.Label = QLabel()
        self.Label.setFixedSize(20, 20)
        layout = QHBoxLayout()
        layout.setContentsMargins(0, 0, 5, 0)
        layout.setSpacing(0)
        layout.addStretch(1)
        layout.addWidget(self.Label)
        self.FolderButton = QPushButton()
        self.FolderButton.setLayout(layout)
        self.FolderButton.clicked.connect(lambda: self.collapse() if self.IsExpanded else self.expand())

        widgetlayout = QGridLayout()
        widgetlayout.setContentsMargins(0, 0, 0, 0)
        widgetlayout.setSpacing(0)
        self.Widget = WidgetBase()
        self.Widget.setAttribute(Qt.WA_StyledBackground)
        self.Widget.setLayout(widgetlayout)
        self.Widget.Resized.connect(self._resizeHeight)

        self.Layout = QVBoxLayout(self)
        self.Layout.setContentsMargins(0, 0, 0, 0)
        self.Layout.setSpacing(0)
        self.Layout.addWidget(self.FolderButton)
        self.Layout.addWidget(self.Widget)

    def _resizeHeight(self, addedWidget: Optional[QWidget] = None):
        ButtonHeight = self.FolderButton.minimumSizeHint().height()
        LayoutSpacing = self.Layout.spacing()
        WidgetLayoutMargins = self.Widget.layout().contentsMargins().top() + self.Widget.layout().contentsMargins().bottom()
        WidgetHeight = (WidgetLayoutMargins + addedWidget.height()) if addedWidget is not None else self.Widget.height()
        AdjustedHeight = ButtonHeight + LayoutSpacing + WidgetHeight if WidgetHeight >=0 else 0
        self.setFixedHeight(AdjustedHeight)

    def addWidget(self, widget: QWidget, title: str):
        self.Widget.layout().addWidget(widget)
        self.FolderButton.setText(title)
        def resizeWidgetHeight():
            AdjustedHeight = widget.height()
            self.Widget.setFixedHeight(AdjustedHeight) if self.IsExpanded else None
        widget.Resized.connect(resizeWidgetHeight) if hasattr(widget, 'Resized') else None

    def expand(self):
        Function_SetWidgetSizeAnimation(self.Widget, TargetHeight = self.Widget.minimumSizeHint().height()).start()
        self.Label.setPixmap(QPixmap(":/ToolBox_Icon/Icons/DownArrow.png").scaled(self.Label.size(), Qt.IgnoreAspectRatio, Qt.SmoothTransformation))
        self.IsExpanded = True

    def collapse(self):
        Function_SetWidgetSizeAnimation(self.Widget, TargetHeight = 0).start()
        self.Label.setPixmap(QPixmap(":/ToolBox_Icon/Icons/LeftArrow.png").scaled(self.Label.size(), Qt.IgnoreAspectRatio, Qt.SmoothTransformation))
        self.IsExpanded = False

    def setText(self, text: str):
        self.FolderButton.setText(text)


class ToolBoxBase(QFrame): #class ToolBoxBase(ScrollAreaBase):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.Pages = []

        self.Layout = QVBoxLayout(self)
        self.Layout.setContentsMargins(0, 0, 0, 0)
        self.Layout.setSpacing(12)
        #self.Layout.addStretch(1)

        StyleSheetBase.ToolBox.Apply(self)

    def addItem(self, widget: QWidget, text: str):
        page = ToolPage(self)
        page.addWidget(widget, text)
        self.Layout.addWidget(page)
        self.Pages.append(page)
        def resizeHeight():
            AdjustedHeight = page.height()
            self.setFixedHeight(AdjustedHeight)
        page.Resized.connect(resizeHeight)

    def widget(self, index: int) -> ToolPage:
        return self.Pages[index]

    def setItemText(self, index: int, text: str) -> None:
        self.widget(index).setText(text)

    def setCurrentIndex(self, index: int) -> None:
        ''''''

    def CurrentIndex(self) -> int:
        ''''''

    def indexOf(self, widget: QWidget) -> int:
        return self.Pages.index(widget if isinstance(widget, ToolPage) else Function_FindParentUI(widget, ToolPage))

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.ToolBox.Deregistrate(self)

##############################################################################################################################

class TableBase(QTableView):
    '''
    '''
    sorted = Signal()

    def __init__(self,
        parent: Optional[QWidget] = None
    ):
        super().__init__(parent)

        self.StandardItemModel = QStandardItemModel(self)
        super().setModel(self.StandardItemModel)

        super().setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        super().verticalHeader().setStretchLastSection(False)
        super().verticalHeader().setResizeContentsPrecision(0)
        super().verticalHeader().setSectionResizeMode(QHeaderView.ResizeToContents)
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
        self.model().rowsInserted.connect(self.SetIndex)
        self.model().rowsRemoved.connect(self.SetIndex)
        self.sorted.connect(self.SetIndex)

        self.IsIndexShown = False
        self.SetIndexHeaderVisible(True)

        StyleSheetBase.Table.Apply(self)

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

    def cellWidget(self, row: int, column: int) -> QWidget:
        return super().indexWidget(self.model().index(row, column + 1))

    def setCellWidget(self, row: int, column: int, widget: QWidget) -> None:
        super().setIndexWidget(self.model().index(row, column + 1), widget)

    def setHorizontalHeaderItem(self, column: int, item: QStandardItem) -> None:
        self.model().setHorizontalHeaderItem(column + 1, item)

    def SetIndex(self) -> None:
        for Index in range(self.model().rowCount()):
            self.model().setItem(Index, 0, QStandardItem(f"{Index + 1}"))

    def SetIndexHeaderVisible(self, ShowIndexHeader: bool = True) -> None:
        if ShowIndexHeader and not self.IsIndexShown:
            super().showColumn(0)
            self.IsIndexShown = True
        if not ShowIndexHeader and self.IsIndexShown:
            super().hideColumn(0)
            self.IsIndexShown = False

    def SetHorizontalHeaders(self, Headers: list[str]) -> None:
        for Index, Header in enumerate(Headers):
            if Index == 1 + self.columnCount():
                return print("Maximum headers reached")
            self.setHorizontalHeaderItem(Index, QStandardItem(Header))

    def SetSectionVerticalResizeMode(self, row: int, mode: QHeaderView.ResizeMode) -> None:
        super().verticalHeader().setSectionResizeMode(row, mode)

    def SetSectionHorizontalResizeMode(self, column: int, mode: QHeaderView.ResizeMode) -> None:
        super().horizontalHeader().setSectionResizeMode(column + 1, mode)

    def SelectOuterRow(self, InnerWidget: QWidget) -> None:
        CellWidget = InnerWidget.parent()
        ModelIndex = self.indexAt(CellWidget.pos())
        self.selectRow(ModelIndex.row()) #if index.isValid() else None

    def AddRow(self, Layouts: list[QLayout], ResizeModes: list[Optional[QHeaderView.ResizeMode]], ColumnWidth: list[Optional[int]], Height: Optional[int]) -> None:
        TargetRow = self.currentRow() + 1
        ColumnCount = self.columnCount()
        self.insertRow(TargetRow)
        for ColumnCount in range(ColumnCount):
            self.setCellWidget(TargetRow, ColumnCount, QWidget())
            self.cellWidget(TargetRow, ColumnCount).setLayout(Layouts[ColumnCount])
            self.SetSectionHorizontalResizeMode(ColumnCount, ResizeModes[ColumnCount]) if ResizeModes[ColumnCount] is not None else None
            self.setColumnWidth(ColumnCount, ColumnWidth[ColumnCount]) if ColumnWidth[ColumnCount] is not None else None
        self.setRowHeight(TargetRow, Height) if Height is not None else None

    def DelRow(self) -> None:
        self.removeRow(self.currentRow()) if self.rowCount() > 1 else None

    def ClearRows(self):
        while self.rowCount() > 0:
            self.removeRow(0)

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Table.Deregistrate(self)

##############################################################################################################################

class LineEdit(QLineEdit):
    '''
    '''
    focusedIn = Signal()
    focusedOut = Signal()

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

    def focusInEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedIn.emit()

    def focusOutEvent(self, arg__1: QFocusEvent) -> None:
        self.focusedOut.emit()


class LineEditBase(QFrame):
    '''
    '''
    textChanged = Signal(str)
    cursorPositionChanged = Signal(int, int)

    focusedIn = Signal()
    focusedOut = Signal()

    interacted = Signal()

    rectChanged = Signal(QRect)

    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.LineEdit = LineEdit()
        self.LineEdit.textChanged.connect(self.textChanged.emit)
        self.LineEdit.textChanged.connect(lambda: self.interacted.emit())
        #self.LineEdit.cursorPositionChanged.connect(self.cursorPositionChanged.emit)
        self.LineEdit.focusedIn.connect(self.focusInEvent)
        self.LineEdit.focusedOut.connect(self.focusOutEvent)

        self.Button = ButtonBase()
        self.Button.setIcon(IconBase.OpenedFolder)
        self.Button.clicked.connect(self.interacted.emit)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(0)
        HBoxLayout.setContentsMargins(0, 0, 0, 0)
        HBoxLayout.addWidget(self.LineEdit)
        HBoxLayout.addWidget(self.Button, alignment = Qt.AlignRight)

        self.ToolTip = QToolTip()
        self.IsAlerted = False

        StyleSheetBase.Edit.Apply(self)

    def focusInEvent(self) -> None:
        self.focusedIn.emit()

    def focusOutEvent(self) -> None:
        self.focusedOut.emit()

    def showToolTip(self, Content: Optional[str] = None) -> None:
        XPos = 0
        YPos = 0 - self.height()
        self.ToolTip.showText(self.mapToGlobal(QPoint(XPos, YPos)), Content) if not self.ToolTip.isVisible() and Content is not None else None

    def hideToolTip(self) -> None:
        self.ToolTip.hideText() if self.ToolTip.isVisible() else None

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        position = event.position()
        self.cursorPositionChanged.emit(position.x(), position.y())
        self.interacted.emit()

    def moveEvent(self, event: QMoveEvent) -> None:
        self.rectChanged.emit(self.rect())

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.rectChanged.emit(self.rect())

    def setAlignment(self, flag: Qt.AlignmentFlag) -> None:
        self.LineEdit.setAlignment(flag)

    def setReadOnly(self, arg__1: bool) -> None:
        self.LineEdit.setReadOnly(arg__1)

    def clear(self) -> None:
        self.LineEdit.clear()

    def text(self) -> str:
        return self.LineEdit.text()

    def setText(self, arg__1: str) -> None:
        return self.LineEdit.setText(arg__1)

    def placeholderText(self) -> str:
        return self.LineEdit.placeholderText()

    def setPlaceholderText(self, arg__1: str) -> None:
        self.LineEdit.setPlaceholderText(arg__1)

    def SetFileDialog(self, Mode: str, FileType: Optional[str] = None, Directory: Optional[str] = None, ButtonTooltip: str = "Browse"):
        self.Button.clicked.connect(
            lambda: self.LineEdit.setText(
                Function_GetFileDialog(
                    Mode = Mode,
                    FileType = FileType,
                    Directory = os.path.expanduser('~/Documents' if platform.system() == "Windows" else '~/') if Directory is None else Directory
                )
            )
        )
        self.Button.setToolTip(ButtonTooltip)

    def RemoveFileDialogButton(self):
        self.Button.deleteLater()
        self.Button.hide()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Edit.Deregistrate(self)

    def setStyleSheet(self, styleSheet: str) -> None:
        AlertStyle = 'LineEditBase {border-color: red;}' if self.IsAlerted else ''
        super().setStyleSheet(styleSheet + AlertStyle)

    def Alert(self, Enable: bool, Content: Optional[str] = None) -> None:
        self.IsAlerted = Enable
        StyleSheetBase.Edit.Apply(self)
        self.showToolTip(Content) if Enable else self.hideToolTip()

##############################################################################################################################

class MediaPlayerBase(QWidget):
    '''
    '''
    def __init__(self, parent: QWidget = None):
        super().__init__(parent)

        self.StackedWidget = QStackedWidget()
        self.StackedWidget.setMaximumSize(36, 36)
        self.StackedWidget.setContentsMargins(0, 0, 0, 0)
        self.PlayButton = ButtonBase()
        self.PlayButton.setIcon(IconBase.Play)
        self.PauseButton = ButtonBase()
        self.PauseButton.setIcon(IconBase.Pause)
        self.PauseButton.clicked.connect(lambda: self.StackedWidget.setCurrentWidget(self.PlayButton))
        self.PlayButton.clicked.connect(lambda: self.StackedWidget.setCurrentWidget(self.PauseButton))
        self.StackedWidget.addWidget(self.PlayButton)
        self.StackedWidget.addWidget(self.PauseButton)
        self.StackedWidget.setCurrentWidget(self.PlayButton)

        self.Slider = QSlider()
        self.Slider.setOrientation(Qt.Horizontal)

        HBoxLayout = QHBoxLayout(self)
        HBoxLayout.setSpacing(12)
        HBoxLayout.setContentsMargins(21, 12, 21, 12)
        HBoxLayout.addWidget(self.StackedWidget, stretch = 1)
        HBoxLayout.addWidget(self.Slider, stretch = 5)

        AudioOutput = QAudioOutput(self)
        self.MediaPlayer = QMediaPlayer()
        self.MediaPlayer.setAudioOutput(AudioOutput)
        #self.MediaPlayer.mediaStatusChanged.connect(lambda Status: self.MediaPlayer.stop() if Status == QMediaPlayer.EndOfMedia else None)

        StyleSheetBase.Player.Apply(self)

    def SetMediaPlayer(self, MediaPath: str):
        self.MediaPlayer.setSource(QUrl.fromLocalFile(MediaPath))

        self.PlayButton.clicked.connect(self.MediaPlayer.play)
        self.PauseButton.clicked.connect(self.MediaPlayer.pause)
        self.MediaPlayer.mediaStatusChanged.connect(lambda status: self.StackedWidget.setCurrentWidget(self.PlayButton) if status == QMediaPlayer.EndOfMedia else None)

        self.Slider.setRange(0, 100)
        self.Slider.sliderMoved.connect(lambda: self.MediaPlayer.setPosition(int(self.Slider.value() / 100 * self.MediaPlayer.duration())))
        self.MediaPlayer.positionChanged.connect(lambda Position: self.Slider.setValue(int(Position / self.MediaPlayer.duration() * 100)))

    def ReleaseMediaPlayer(self):
        self.MediaPlayer.stop()
        self.MediaPlayer.setSource('')
        #self.MediaPlayer.deleteLater()

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Player.Deregistrate(self)

##############################################################################################################################