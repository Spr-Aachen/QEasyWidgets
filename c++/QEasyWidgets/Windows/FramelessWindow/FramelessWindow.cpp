#include "FramelessWindow.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPainter>
#include <QStyle>
#include <QWindow>
#include <QScreen>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

namespace QEW {

// FramelessWindowBase implementation
FramelessWindowBase::FramelessWindowBase(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , m_titleBar(nullptr)
    , m_mainLayout(nullptr)
    , m_windowState(Normal)
    , m_stretchable(true)
    , m_dragging(false)
    , m_borderWidth(8)
    , m_resizing(false)
    , m_resizeDirection(0)
    , m_maskWidget(nullptr)
{
    setupUI();
}

void FramelessWindowBase::setupUI()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_titleBar = new TitleBarBase(this);
    m_mainLayout->addWidget(m_titleBar);

    // TitleBarBase handles window control internally, no need to connect signals
}

void FramelessWindowBase::setMinimumSize(int width, int height)
{
    QWidget::setMinimumSize(width, height);
}

void FramelessWindowBase::setFrameless(bool stretchable)
{
    m_stretchable = stretchable;
    if (!stretchable) {
        m_titleBar->minimizeButton()->hide();
        m_titleBar->maximizeButton()->hide();
    }
}

void FramelessWindowBase::showMask(bool show)
{
    if (show) {
        if (!m_maskWidget) {
            m_maskWidget = new QWidget(parentWidget());
            m_maskWidget->setStyleSheet("background-color: rgba(0, 0, 0, 100);");
            m_maskWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
            m_maskWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        }
        m_maskWidget->setGeometry(parentWidget()->geometry());
        m_maskWidget->show();
        m_maskWidget->raise();
    } else {
        if (m_maskWidget) {
            m_maskWidget->hide();
        }
    }
}

void FramelessWindowBase::setWindowState(WindowState state)
{
    if (m_windowState == state) return;

    if (state == Maximized) {
        maximizeWindow();
    } else if (state == Normal) {
        restoreWindow();
    }

    m_windowState = state;
}

void FramelessWindowBase::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    emit showed();
}

void FramelessWindowBase::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed();
}

void FramelessWindowBase::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_maskWidget && m_maskWidget->isVisible()) {
        m_maskWidget->setGeometry(parentWidget()->geometry());
    }
}

void FramelessWindowBase::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // Draw border if not maximized
    if (m_windowState != Maximized) {
        QPainter painter(this);
        painter.setPen(QColor(200, 200, 200));
        painter.drawRect(0, 0, width() - 1, height() - 1);
    }
}

void FramelessWindowBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_stretchable) {
        m_dragStartPosition = event->globalPos() - pos();

        // Check if we're on a border for resizing
        QPoint pos = event->pos();
        m_resizeDirection = 0;

        if (pos.x() <= m_borderWidth) m_resizeDirection |= 0x01; // Left
        if (pos.x() >= width() - m_borderWidth) m_resizeDirection |= 0x02; // Right
        if (pos.y() <= m_borderWidth) m_resizeDirection |= 0x04; // Top
        if (pos.y() >= height() - m_borderWidth) m_resizeDirection |= 0x08; // Bottom

        if (m_resizeDirection != 0) {
            m_resizing = true;
        } else {
            m_dragging = true;
        }
    }
    QWidget::mousePressEvent(event);
}

void FramelessWindowBase::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - m_dragStartPosition;
        move(newPos);
    } else if (m_resizing && (event->buttons() & Qt::LeftButton)) {
        // Handle resizing (simplified)
        QPoint globalPos = event->globalPos();
        QRect newGeometry = geometry();

        if (m_resizeDirection & 0x01) { // Left
            newGeometry.setLeft(globalPos.x());
        }
        if (m_resizeDirection & 0x02) { // Right
            newGeometry.setRight(globalPos.x());
        }
        if (m_resizeDirection & 0x04) { // Top
            newGeometry.setTop(globalPos.y());
        }
        if (m_resizeDirection & 0x08) { // Bottom
            newGeometry.setBottom(globalPos.y());
        }

        setGeometry(newGeometry);
    } else {
        // Update cursor for resize directions
        QPoint pos = event->pos();
        int direction = 0;

        if (pos.x() <= m_borderWidth) direction |= 0x01;
        if (pos.x() >= width() - m_borderWidth) direction |= 0x02;
        if (pos.y() <= m_borderWidth) direction |= 0x04;
        if (pos.y() >= height() - m_borderWidth) direction |= 0x08;

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

void FramelessWindowBase::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
    m_resizing = false;
    m_resizeDirection = 0;
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
}

bool FramelessWindowBase::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_NCCALCSIZE) {
        *result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void FramelessWindowBase::onMinimizeRequested()
{
    showMinimized();
}

void FramelessWindowBase::onMaximizeRequested()
{
    if (m_windowState == Maximized) {
        restoreWindow();
    } else {
        maximizeWindow();
    }
}

void FramelessWindowBase::onCloseRequested()
{
    close();
}

void FramelessWindowBase::updateWindowGeometry()
{
    if (m_windowState == Maximized) {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        setGeometry(screenGeometry);
    }
}

void FramelessWindowBase::restoreWindow()
{
    if (m_windowState == Maximized) {
        setGeometry(m_normalGeometry);
        m_windowState = Normal;
        // TitleBarBase handles icon updates internally
    }
}

void FramelessWindowBase::maximizeWindow()
{
    if (m_windowState == Normal) {
        m_normalGeometry = geometry();
        updateWindowGeometry();
        m_windowState = Maximized;
        // TitleBarBase handles icon updates internally
    }
}

} // namespace QEW
