import win32gui
import win32con
import win32api
import win32print
from typing import Optional
from ctypes import Structure, c_int, POINTER, WinDLL, byref
from ctypes.wintypes import UINT, HWND, RECT, MSG, LPRECT
from PySide6.QtCore import Qt, Signal, QPoint, QRect, QEvent
from PySide6.QtGui import QFont, QCursor, QMouseEvent, QShowEvent, QCloseEvent, QMoveEvent, QResizeEvent
from PySide6.QtWidgets import QApplication, QWidget, QLabel

from ...Common.StyleSheet import StyleSheetBase
from ...Common.QFunctions import *
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

##############################################################################################################################

def IsWindowMaximized(hWnd: int):
    WindowPlacement = win32gui.GetWindowPlacement(hWnd)

    Result = WindowPlacement[1] == win32con.SW_MAXIMIZE if WindowPlacement else False

    return Result


def IsWindowFullScreen(hWnd: int):
    hWnd = int(hWnd)

    WindowRect = win32gui.GetWindowRect(hWnd)

    hMonitor = win32api.MonitorFromWindow(hWnd, win32con.MONITOR_DEFAULTTOPRIMARY)
    MonitorInfo = win32api.GetMonitorInfo(hMonitor)

    Result = all(w == m for w, m in zip(WindowRect, MonitorInfo["Monitor"])) if WindowRect and MonitorInfo else False

    return Result


def GetSystemMetrics(hWnd: int, index: int, dpiScaling: bool):
    if hasattr(windll.user32, 'GetSystemMetricsForDpi'):
        if hasattr(windll.user32, 'GetDpiForWindow'):
            dpi = windll.user32.GetDpiForWindow(hWnd)
        else:
            dpi = 96
            hdc = win32gui.GetDC(hWnd)
            if hdc:
                dpiX = win32print.GetDeviceCaps(hdc, win32con.LOGPIXELSX)
                dpiY = win32print.GetDeviceCaps(hdc, win32con.LOGPIXELSY)
                if dpiX > 0 and dpiScaling:
                    dpi = dpiX
                if dpiY > 0 and not dpiScaling:
                    dpi = dpiY
                win32gui.ReleaseDC(hWnd, hdc)
        return windll.user32.GetSystemMetricsForDpi(index, dpi)

    else:
        return win32api.GetSystemMetrics(index)

##############################################################################################################################

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
        self.TitleBar = TitleBarBase(self)

        self.Mask = QLabel(self)
        self.rectChanged.connect(self.Mask.setGeometry)
        self.Mask.setStyleSheet('background-color: rgba(0, 0, 0, 111);')
        self.Mask.setAlignment(Qt.AlignCenter)
        self.Mask.setFont(QFont('Microsoft YaHei', int(min_height / 10), QFont.Bold))
        self.Mask.hide()

        self.resize(min_width, min_height)

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
        if self._check_ifdraggable(event.position()) == True and event.buttons() == Qt.MouseButton.LeftButton:
            self.setCursor(Qt.CursorShape.OpenHandCursor)
            self._move_window(event.position())

    def mousePressEvent(self, event: QMouseEvent) -> None:
            return

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
        self.TitleBar.resize(self.width(), self.TitleBar.height()) if isinstance(self.TitleBar, TitleBarBase) else None
        self.setCursor(Qt.CursorShape.ArrowCursor)

    def nativeEvent(self, eventType, message):
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

##############################################################################################################################