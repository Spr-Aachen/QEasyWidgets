#include "framelessWindow.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPainter>
#include <QStyle>
#include <QWindow>
#include <QScreen>
#include <QVBoxLayout>


/**
 * Linux-specific FramelessWindowBase implementation
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
    
    if (!stretchable) {
        m_edgeSize = 0;
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

    // Draw border if not maximized
    if (m_windowState != Maximized) {
        QPainter painter(this);
        painter.setPen(QColor(200, 200, 200));
        painter.drawRect(0, 0, width() - 1, height() - 1);
    }
}

void FramelessWindowBase::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_stretchable) {
        if (checkIfDraggable(event->pos())) {
            setCursor(Qt::OpenHandCursor);
            windowHandle()->startSystemMove();
            QApplication::instance()->postEvent(
                windowHandle(),
                new QMouseEvent(QEvent::MouseButtonRelease, QPoint(-1, -1), Qt::LeftButton, Qt::NoButton, Qt::NoModifier)
            );
        }
    }
    QWidget::mousePressEvent(event);
}

void FramelessWindowBase::mouseMoveEvent(QMouseEvent *event) {
    // Linux simplification: just update cursor
    QWidget::mouseMoveEvent(event);
}

void FramelessWindowBase::mouseReleaseEvent(QMouseEvent *event) {
    if (checkIfDraggable(event->pos())) {
        setCursor(Qt::ArrowCursor);
        windowHandle()->startSystemResize(Qt::Edges());
    }
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