import win32gui
import win32con
from ctypes import Structure, c_int, POINTER, WinDLL, byref
from ctypes.wintypes import UINT, HWND, RECT, MSG, LPRECT
from PySide6.QtCore import Qt, QPoint, QEvent, QEventLoop
from PySide6.QtGui import QFont, QCursor, QMouseEvent, QShowEvent, QCloseEvent, QMoveEvent, QResizeEvent
from PySide6.QtWidgets import QWidget, QMainWindow

from ..Common.QFunctions import *
from ..Resources.Sources import *
from .Bar import TitleBarBase

##############################################################################################################################

class MARGINS(Structure):
    '''
    typedef struct _MARGINS {
        int cxLeftWidth;
        int cxRightWidth;
        int cyTopHeight;
        int cyBottomHeight;
    } MARGINS, *PMARGINS;
    '''
    _fields_ = [
        ("cxLeftWidth",    c_int),
        ("cxRightWidth",   c_int),
        ("cyTopHeight",    c_int),
        ("cyBottomHeight", c_int),
    ]


PMARGINS = POINTER(MARGINS)


class WINDOWPOS(Structure):
    '''
    typedef struct tagWINDOWPOS {
        HWND hwnd;
        HWND hwndInsertAfter;
        int  x;
        int  y;
        int  cx;
        int  cy;
        UINT flags;
    } WINDOWPOS, *LPWINDOWPOS, *PWINDOWPOS;
    '''
    _fields_ = [
        ('hwnd',            HWND),
        ('hwndInsertAfter', HWND),
        ('x',               c_int),
        ('y',               c_int),
        ('cx',              c_int),
        ('cy',              c_int),
        ('flags',           UINT)
    ]


PWINDOWPOS = POINTER(WINDOWPOS)


class NCCALCSIZE_PARAMS(Structure):
    '''
    typedef struct tagNCCALCSIZE_PARAMS {
        RECT       rgrc[3];
        PWINDOWPOS lppos;
    } NCCALCSIZE_PARAMS, *LPNCCALCSIZE_PARAMS;
    '''
    _fields_ = [
        ('rgrc',  RECT*3),
        ('lppos', PWINDOWPOS)
    ]


class WindowBase:
    '''
    '''
    showed = Signal()
    closed = Signal()

    rectChanged = Signal(QRect)

    edge_size = 3 # 窗体边缘尺寸（出现缩放标记的范围）

    def __init__(self,
        min_width = 630, # 窗体的最小宽度
        min_height = 420, # 窗体的最小高度
    ):
        self.resize(min_width, min_height)

        self.TitleBar = TitleBarBase(self)

        self.Mask = QLabel(self)
        self.rectChanged.connect(self.Mask.setGeometry)
        self.Mask.setStyleSheet('background-color: rgba(0, 0, 0, 111);')
        self.Mask.setAlignment(Qt.AlignCenter)
        self.Mask.setFont(QFont('Microsoft YaHei', int(min_height / 10), QFont.Bold))
        self.Mask.hide()

        self.System = platform.system()

    def _check_ifdraggable(self, pos) -> bool:
        return (0 < pos.x() < self.width() and 0 < pos.y() < self.TitleBar.height()) if self.TitleBar is not None else False

    def _move_window(self, pos) -> None:
        self.windowHandle().startSystemMove()
        QApplication.instance().postEvent(
            self.windowHandle(),
            QMouseEvent(
                QEvent.MouseButtonRelease,
                QPoint(-1, -1),
                Qt.LeftButton,
                Qt.NoButton,
                Qt.NoModifier
            )
        )

    def _resize_window(self, pos, edges) -> None:
        self.windowHandle().startSystemResize(edges) if edges is not None else None

    def mouseMoveEvent(self, event: QMouseEvent) -> None:
        if self.System != 'Windows':
            return
        if self._check_ifdraggable(event.position()) == True and event.buttons() == Qt.MouseButton.LeftButton:
            self.setCursor(Qt.CursorShape.OpenHandCursor)
            self._move_window(event.position())

    def mousePressEvent(self, event: QMouseEvent) -> None:
        if self.System == 'Windows':
            return
        if self._check_ifdraggable(event.position()) == True and event.buttons() == Qt.MouseButton.LeftButton:
            self.setCursor(Qt.CursorShape.OpenHandCursor)
            self._move_window(event.position())

    def mouseReleaseEvent(self, event: QMouseEvent) -> None:
        if self._check_ifdraggable(event.position()) == True:
            self.setCursor(Qt.CursorShape.ArrowCursor)
            self._resize_window(event.position(), None)

    def mouseDoubleClickEvent(self, event: QMouseEvent) -> None:
        if self._check_ifdraggable(event.position()) == True and event.buttons() == Qt.MouseButton.LeftButton:
            self.showNormal() if self.isMaximized() else self.showMaximized() #self.setWindowState(Qt.WindowState.WindowMaximized)

    def showEvent(self, event: QShowEvent) -> None:
        self.showed.emit()
        event.accept()

    def closeEvent(self, event: QCloseEvent) -> None:
        self.closed.emit()
        event.accept()

    def moveEvent(self, event: QMoveEvent) -> None:
        self.rectChanged.emit(self.rect())

    def resizeEvent(self, event: QResizeEvent) -> None:
        self.rectChanged.emit(self.rect())
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def nativeEvent(self, eventType, message):
        if self.System != 'Windows':
            if eventType.type() in (QEvent.MouseButtonPress, QEvent.MouseMove):
                border_width = self.edge_size
                left   = (eventType.globalPos() - self.pos()).x() < border_width
                top    = (eventType.globalPos() - self.pos()).y() < border_width
                right  = (eventType.globalPos() - self.pos()).x() > self.width() - border_width
                bottom = (eventType.globalPos() - self.pos()).y() > self.height() - border_width
                if True not in (left, top, right, bottom):
                    pass
                elif eventType.type() == QEvent.MouseButtonPress and self.windowState() != Qt.WindowNoState:
                    self._move_window(eventType.globalPos())
            return False
        Message = MSG.from_address(int(message))
        if Message.message == win32con.WM_NCCALCSIZE:
            if Message.wParam != 0:
                Rect = NCCALCSIZE_PARAMS.from_address(Message.lParam).rgrc[0]
                MissingHBorderPixels, MissingVBorderPixels = GetMissingBorderPixels(Message.hWnd) if IsWindowMaximized(Message.hWnd) and not IsWindowFullScreen(Message.hWnd) else (0, 0)
                Rect.left += MissingHBorderPixels
                Rect.top += MissingVBorderPixels
                Rect.right -= MissingHBorderPixels
                Rect.bottom -= MissingVBorderPixels
                return True, win32con.WVR_REDRAW
            else:
                Rect = LPRECT.from_address(Message.lParam)
                return True, 0
        if Message.message == win32con.WM_NCHITTEST:
            border_width = self.edge_size if not IsWindowMaximized(Message.hWnd) and not IsWindowFullScreen(Message.hWnd) else 0
            left   = QCursor.pos().x() - self.x() < border_width
            top    = QCursor.pos().y() - self.y() < border_width
            right  = QCursor.pos().x() - self.x() > self.frameGeometry().width() - border_width
            bottom = QCursor.pos().y() - self.y() > self.frameGeometry().height() - border_width
            if True not in (left, top, right, bottom):
                pass
            elif left and top:
                return True, win32con.HTTOPLEFT
            elif left and bottom:
                return True, win32con.HTBOTTOMLEFT
            elif right and top:
                return True, win32con.HTTOPRIGHT
            elif right and bottom:
                return True, win32con.HTBOTTOMRIGHT
            elif left:
                return True, win32con.HTLEFT
            elif top:
                return True, win32con.HTTOP
            elif right:
                return True, win32con.HTRIGHT
            elif bottom:
                return True, win32con.HTBOTTOM
        return QWidget.nativeEvent(self, eventType, message)

    def setFrameless(self, SetStrechable: bool = True, SetDropShadowEffect: bool = True) -> None:
        self.setWindowFlags(self.windowFlags() | Qt.FramelessWindowHint)
        if self.System != 'Windows':
            return
        hWnd = self.winId()
        Index = win32con.GWL_STYLE
        Value = win32gui.GetWindowLong(hWnd, Index)
        win32gui.SetWindowLong(hWnd, Index, Value | win32con.WS_THICKFRAME | win32con.WS_CAPTION)
        if not SetStrechable:
            self.edge_size = 0
        if SetDropShadowEffect:
            ExtendFrameIntoClientArea = WinDLL("dwmapi").DwmExtendFrameIntoClientArea
            ExtendFrameIntoClientArea.argtypes = [c_int, PMARGINS]
            ExtendFrameIntoClientArea(hWnd, byref(MARGINS(-1, -1, -1, -1)))

    def setTitleBar(self, TitleBar: Optional[QWidget]) -> None:
        try:
            self.TitleBar.deleteLater()
            self.TitleBar.hide()
            StyleSheetBase.Bar.Deregistrate(self.TitleBar)
        except:
            pass
        if TitleBar is not None:
            self.TitleBar = TitleBar
            self.TitleBar.setParent(self) if self.TitleBar.parent() is None else None
            self.TitleBar.raise_() if self.TitleBar.isHidden() else None
        else:
            self.TitleBar = None

    def ShowMask(self, SetVisible: bool, MaskContent: Optional[str] = None) -> None:
        if SetVisible:
            self.Mask.raise_() if self.Mask.isHidden() else None
            self.Mask.setText(MaskContent) if MaskContent is not None else self.Mask.clear()
            self.Mask.show()
        else:
            self.Mask.clear()
            self.Mask.hide()


class MainWindowBase(WindowBase, QMainWindow):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None,
        flags: Qt.WindowType = Qt.Window | Qt.WindowSystemMenuHint | Qt.WindowMinMaxButtonsHint,
        min_width: int = 1280,
        min_height: int = 720
    ):
        QMainWindow.__init__(self, parent, flags)
        WindowBase.__init__(self, min_width, min_height)

        self.setFrameless()

        self.CentralLayout = QGridLayout()
        self.CentralWidget = QWidget(self)
        self.CentralWidget.setObjectName('CentralWidget')
        self.CentralWidget.setLayout(self.CentralLayout)
        self.setCentralWidget(self.CentralWidget)

        StyleSheetBase.Window.Apply(self)

    def setCentralWidget(self, CentralWidget: Optional[QWidget]) -> None:
        try:
            super().takeCentralWidget(self.CentralWidget)
            self.CentralWidget.deleteLater()
            self.CentralWidget.hide()
            self.ClearDefaultStyleSheet()
        except:
            pass
        if CentralWidget is not None:
            self.CentralWidget = CentralWidget
            super().setCentralWidget(self.CentralWidget)
            self.CentralWidget.setParent(self) if self.CentralWidget.parent() is None else None
            self.CentralWidget.raise_() if self.CentralWidget.isHidden() else None
        else:
            self.CentralWidget = None

    def setStyleSheet(self, styleSheet: str) -> None:
        super().setStyleSheet(styleSheet.replace('#CentralWidget', f'#{self.CentralWidget.objectName()}'))

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Window.Deregistrate(self)


class ChildWindowBase(WindowBase, QWidget):
    '''
    '''
    def __init__(self,
        parent: Optional[QWidget] = None,
        f: Qt.WindowType = Qt.Widget,
        min_width: int = 630,
        min_height: int = 420
    ):
        QWidget.__init__(self, None, f) #QWidget.__init__(self, parent, f)
        WindowBase.__init__(self, min_width, min_height)

        self.setFrameless()

        self.setWindowModality(Qt.ApplicationModal)

        self.EventLoop = QEventLoop(self)

        self.showed.connect(lambda: parent.ShowMask(True)) if isinstance(parent, WindowBase) else None
        self.closed.connect(lambda: parent.ShowMask(False)) if isinstance(parent, WindowBase) else None

        StyleSheetBase.Window.Apply(self)

    def exec(self) -> int:
        self.show()
        Result = self.EventLoop.exec()
        self.closed.emit()
        return Result

    def closeEvent(self, event: QCloseEvent) -> None:
        Result = self.EventLoop.exit()
        super().closeEvent(event)

    def setStyleSheet(self, styleSheet: str) -> None:
        super().setStyleSheet(styleSheet.replace('#CentralWidget', f'#{self.objectName()}'))

    def ClearDefaultStyleSheet(self) -> None:
        StyleSheetBase.Window.Deregistrate(self)

##############################################################################################################################