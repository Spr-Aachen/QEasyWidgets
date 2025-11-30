#include "framelessWindow.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPainter>
#include <QStyle>
#include <QWindow>
#include <QScreen>
#include <QVBoxLayout>
#include <windows.h>
#include <windowsx.h>
#include <cmath>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#ifndef SM_CXPADDEDBORDER
#define SM_CXPADDEDBORDER 92
#endif


// Helper typedefs for runtime DPI-aware system metric functions
using GetSystemMetricsForDpi_t = int (WINAPI*)(int, UINT);
using GetDpiForWindow_t = UINT (WINAPI*)(HWND);


// Get system metric adjusted for the window's DPI when available, otherwise fall back.
static int getSystemMetricsForWindow(HWND hWnd, int index, bool dpiScaling) {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    GetSystemMetricsForDpi_t pGetSystemMetricsForDpi = nullptr;
    GetDpiForWindow_t pGetDpiForWindow = nullptr;

    if (user32) {
        pGetSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpi_t>(
            GetProcAddress(user32, "GetSystemMetricsForDpi")
        );
        pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
            GetProcAddress(user32, "GetDpiForWindow")
        );
    }

    if (pGetSystemMetricsForDpi) {
        UINT dpi = 96;
        if (pGetDpiForWindow) {
            dpi = pGetDpiForWindow(hWnd);
        } else {
            HDC hdc = GetDC(hWnd);
            if (hdc) {
                int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
                int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
                if (dpiX > 0 && dpiScaling) dpi = dpiX;
                if (dpiY > 0 && !dpiScaling) dpi = dpiY;
                ReleaseDC(hWnd, hdc);
            }
        }
        return pGetSystemMetricsForDpi(index, dpi);
    }

    return GetSystemMetrics(index);
}


// Returns true when the window occupies the full monitor (true fullscreen)
static bool isWindowFullScreen(HWND hWnd) {
    RECT wr = {0};
    if (!GetWindowRect(hWnd, &wr)) return false;
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    if (!hMon) return false;
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) return false;
    return (wr.left == mi.rcMonitor.left && wr.top == mi.rcMonitor.top &&
            wr.right == mi.rcMonitor.right && wr.bottom == mi.rcMonitor.bottom);
}


// Query DWM composition state (Aero) safely
static bool isCompositionEnabled() {
    BOOL composition = FALSE;
    if (SUCCEEDED(DwmIsCompositionEnabled(&composition))) {
        return composition != FALSE;
    }
    return false;
}


// Helper macros for extracting coordinates from lParam
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif


/**
 * Windows-specific FramelessWindowBase implementation
 */

FramelessWindowBase::FramelessWindowBase(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , m_titleBar(nullptr)
    , m_mainLayout(nullptr)
    , m_maskWidget(nullptr)
    , m_windowState(Normal)
    , m_stretchable(true)
    , m_dragging(false)
    , m_edgeSize(3)
    , m_resizing(false)
    , m_resizeDirection(0) {
    setupUI();

    // Create animation helper objects as members (composition) instead of multiple QObject
    // inheritance which is not supported by Qt.
    m_backgroundAnimation = new BackgroundColorAnimationBase(this);
    m_textAnimation = new TextColorAnimationBase(this);
}

void FramelessWindowBase::setupUI() {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_titleBar = new TitleBarBase(this);
    m_mainLayout->addWidget(m_titleBar);

    // Create mask widget for modal dialogs
    m_maskWidget = new QLabel(this);
    m_maskWidget->setStyleSheet("background-color: rgba(0, 0, 0, 111);");
    m_maskWidget->setAlignment(Qt::AlignCenter);
    m_maskWidget->hide();
}

void FramelessWindowBase::setMinimumSize(int width, int height) {
    QWidget::setMinimumSize(width, height);
}

void FramelessWindowBase::setFrameless(bool stretchable, bool dropShadowEffect) {
    m_stretchable = stretchable;
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    
    HWND hWnd = reinterpret_cast<HWND>(winId());
    
    // Set window style to allow resizing and custom frame
    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    SetWindowLong(hWnd, GWL_STYLE, style | WS_THICKFRAME | WS_CAPTION);
    
    // Hide minimize/maximize buttons if not stretchable
    if (!stretchable) {
        m_edgeSize = 0;
    }
    
    // Apply DWM drop shadow effect for Windows Vista and later
    if (dropShadowEffect) {
        MARGINS margins = {-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hWnd, &margins);
    }
}

void FramelessWindowBase::setTitleBar(TitleBarBase *titleBar) {
    if (m_titleBar) {
        m_titleBar->deleteLater();
        m_titleBar->hide();
    }
    
    if (titleBar != nullptr) {
        m_titleBar = titleBar;
        m_titleBar->setParent(this);
        m_titleBar->show();
        m_titleBar->raise();
    } else {
        m_titleBar = nullptr;
    }
}

void FramelessWindowBase::showMask(bool show, const QString &maskContent) {
    if (show) {
        if (m_maskWidget) {
            m_maskWidget->raise();
            m_maskWidget->setText(maskContent);
            m_maskWidget->show();
            m_maskWidget->setGeometry(rect());
        }
    } else {
        if (m_maskWidget) {
            m_maskWidget->clear();
            m_maskWidget->hide();
        }
    }
}

void FramelessWindowBase::setWindowState(WindowState state) {
    if (m_windowState == state) return;

    if (state == Maximized) {
        maximizeWindow();
    } else if (state == Normal) {
        restoreWindow();
    }

    m_windowState = state;
}

bool FramelessWindowBase::checkIfDraggable(const QPoint &pos) const {
    return (0 < pos.x() && pos.x() < width() && 
            0 < pos.y() && pos.y() < m_titleBar->height()) 
            ? true : false;
}

void FramelessWindowBase::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    emit showed();
}

void FramelessWindowBase::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent(event);
    emit closed();
}

void FramelessWindowBase::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit rectChanged(rect());
    
    if (m_titleBar) {
        m_titleBar->resize(width(), m_titleBar->height());
    }
    
    if (m_maskWidget && m_maskWidget->isVisible()) {
        m_maskWidget->setGeometry(rect());
    }
    
    setCursor(Qt::ArrowCursor);
}

void FramelessWindowBase::moveEvent(QMoveEvent *event) {
    QWidget::moveEvent(event);
    emit rectChanged(rect());
}

void FramelessWindowBase::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    // Draw border if not maximized and not fullscreen
    if (m_windowState != Maximized) {
        QPainter painter(this);
        painter.setPen(QColor(200, 200, 200));
        painter.drawRect(0, 0, width() - 1, height() - 1);
    }
}

void FramelessWindowBase::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_stretchable) {
        m_dragStartPosition = event->globalPosition().toPoint() - pos();
        
        // Check for resize on borders
        QPoint localPos = event->pos();
        m_resizeDirection = 0;
        
        if (localPos.x() <= m_edgeSize) m_resizeDirection |= 0x01;  // Left
        if (localPos.x() >= width() - m_edgeSize) m_resizeDirection |= 0x02;  // Right
        if (localPos.y() <= m_edgeSize) m_resizeDirection |= 0x04;  // Top
        if (localPos.y() >= height() - m_edgeSize) m_resizeDirection |= 0x08;  // Bottom
        
        if (m_resizeDirection != 0) {
            m_resizing = true;
        } else if (checkIfDraggable(localPos)) {
            m_dragging = true;
        }
    }
    QWidget::mousePressEvent(event);
}

void FramelessWindowBase::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        setCursor(Qt::OpenHandCursor);
        QPoint newPos = event->globalPosition().toPoint() - m_dragStartPosition;
        move(newPos);
    } else if (m_resizing && (event->buttons() & Qt::LeftButton)) {
        QPoint globalPos = event->globalPosition().toPoint();
        QRect newGeometry = geometry();
        
        if (m_resizeDirection & 0x01) {  // Left
            newGeometry.setLeft(globalPos.x());
        }
        if (m_resizeDirection & 0x02) {  // Right
            newGeometry.setRight(globalPos.x());
        }
        if (m_resizeDirection & 0x04) {  // Top
            newGeometry.setTop(globalPos.y());
        }
        if (m_resizeDirection & 0x08) {  // Bottom
            newGeometry.setBottom(globalPos.y());
        }
        
        setGeometry(newGeometry);
    } else {
        // Update cursor based on position
        QPoint localPos = event->pos();
        int direction = 0;
        
        if (localPos.x() <= m_edgeSize) direction |= 0x01;
        if (localPos.x() >= width() - m_edgeSize) direction |= 0x02;
        if (localPos.y() <= m_edgeSize) direction |= 0x04;
        if (localPos.y() >= height() - m_edgeSize) direction |= 0x08;
        
        if (direction == 0x01 || direction == 0x02) {
            setCursor(Qt::SizeHorCursor);
        } else if (direction == 0x04 || direction == 0x08) {
            setCursor(Qt::SizeVerCursor);
        } else if (direction == 0x05 || direction == 0x0A) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (direction == 0x06 || direction == 0x09) {
            setCursor(Qt::SizeFDiagCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
    QWidget::mouseMoveEvent(event);
}

void FramelessWindowBase::mouseReleaseEvent(QMouseEvent *event) {
    if (checkIfDraggable(event->pos())) {
        setCursor(Qt::ArrowCursor);
    }
    m_dragging = false;
    m_resizing = false;
    m_resizeDirection = 0;
    QWidget::mouseReleaseEvent(event);
}

void FramelessWindowBase::mouseDoubleClickEvent(QMouseEvent *event) {
    if (checkIfDraggable(event->pos()) && (event->buttons() & Qt::LeftButton)) {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

bool FramelessWindowBase::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    MSG *msg = static_cast<MSG *>(message);
    
    if (msg->message == WM_NCCALCSIZE) {
        if (msg->wParam) {
            // Adjust client rect when maximized to account for missing border pixels
            // (similar to Python implementation: handle DPI, DWM, and padding borders)
            NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
            RECT &rc = params->rgrc[0];

            HWND hWnd = reinterpret_cast<HWND>(winId());
            BOOL maximized = IsZoomed(hWnd);
            if (maximized && !isWindowFullScreen(hWnd)) {
                int missingH = getSystemMetricsForWindow(hWnd, SM_CXSIZEFRAME, true)
                             + getSystemMetricsForWindow(hWnd, SM_CXPADDEDBORDER, true);
                int missingV = getSystemMetricsForWindow(hWnd, SM_CYSIZEFRAME, false)
                             + getSystemMetricsForWindow(hWnd, SM_CXPADDEDBORDER, false);

                if (missingH <= 0) {
                    double dpr = devicePixelRatioF();
                    missingH = static_cast<int>(std::round((isCompositionEnabled() ? 6.0 : 3.0) * dpr));
                }
                if (missingV <= 0) {
                    double dpr = devicePixelRatioF();
                    missingV = static_cast<int>(std::round((isCompositionEnabled() ? 6.0 : 3.0) * dpr));
                }

                rc.left   += missingH;
                rc.top    += missingV;
                rc.right  -= missingH;
                rc.bottom -= missingV;
            }

            *result = 0;
            return true;
        } else {
            *result = 0;
            return true;
        }
    }
    
    if (msg->message == WM_NCHITTEST) {
        if (!m_stretchable || isMaximized()) {
            return QWidget::nativeEvent(eventType, message, result);
        }
        
        int border_width = m_edgeSize;
        int x = GET_X_LPARAM(msg->lParam) - frameGeometry().left();
        int y = GET_Y_LPARAM(msg->lParam) - frameGeometry().top();
        
        bool left = x < border_width;
        bool top = y < border_width;
        bool right = x > frameGeometry().width() - border_width;
        bool bottom = y > frameGeometry().height() - border_width;
        
        if (left && top) {
            *result = HTTOPLEFT;
            return true;
        } else if (left && bottom) {
            *result = HTBOTTOMLEFT;
            return true;
        } else if (right && top) {
            *result = HTTOPRIGHT;
            return true;
        } else if (right && bottom) {
            *result = HTBOTTOMRIGHT;
            return true;
        } else if (left) {
            *result = HTLEFT;
            return true;
        } else if (top) {
            *result = HTTOP;
            return true;
        } else if (right) {
            *result = HTRIGHT;
            return true;
        } else if (bottom) {
            *result = HTBOTTOM;
            return true;
        }
    }
    
    return QWidget::nativeEvent(eventType, message, result);
}

void FramelessWindowBase::onMinimizeRequested() {
    showMinimized();
}

void FramelessWindowBase::onMaximizeRequested() {
    if (m_windowState == Maximized) {
        restoreWindow();
    } else {
        maximizeWindow();
    }
}

void FramelessWindowBase::onCloseRequested() {
    close();
}

void FramelessWindowBase::updateWindowGeometry() {
    if (m_windowState == Maximized) {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        setGeometry(screenGeometry);
    }
}

void FramelessWindowBase::restoreWindow() {
    if (m_windowState == Maximized) {
        setGeometry(m_normalGeometry);
        m_windowState = Normal;
    }
}

void FramelessWindowBase::maximizeWindow() {
    if (m_windowState == Normal) {
        m_normalGeometry = geometry();
        updateWindowGeometry();
        m_windowState = Maximized;
    }
}