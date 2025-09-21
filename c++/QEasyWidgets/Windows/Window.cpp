#include "Window.h"

#include <QApplication>
#include <QScreen>

#include "../Common/QFunctions.h"


/**
 * MainWindowBase implementation
 */

MainWindowBase::MainWindowBase(QWidget *parent, Qt::WindowFlags flags, int minWidth, int minHeight)
    : FramelessWindowBase(parent, flags)
    , m_centralLayout(nullptr)
    , m_centralWidget(nullptr) {
    setupUI();
    setFrameless(true);
    FramelessWindowBase::setMinimumSize(minWidth, minHeight);

    // Center on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - minWidth) / 2;
    int y = (screenGeometry.height() - minHeight) / 2;
    FramelessWindowBase::move(x, y);
}

void MainWindowBase::setupUI() {
    FramelessWindowBase::setupUI();

    m_centralLayout = new QGridLayout();
    m_centralWidget = new QWidget(this);
    m_centralWidget->setLayout(m_centralLayout);

    // Add central widget to the frameless layout
    FramelessWindowBase::layout()->addWidget(m_centralWidget);
}

void MainWindowBase::setCentralWidget(QWidget *centralWidget) {
    // Remove old central widget
    if (m_centralWidget) {
        FramelessWindowBase::layout()->removeWidget(m_centralWidget);
        m_centralWidget->deleteLater();
        m_centralWidget->hide();
    }

    if (centralWidget) {
        m_centralWidget = centralWidget;
        FramelessWindowBase::layout()->addWidget(m_centralWidget);
        
        // Set parent if needed
        if (!m_centralWidget->parent()) {
            m_centralWidget->setParent(this);
        }
        
        // Show if hidden
        if (m_centralWidget->isHidden()) {
            m_centralWidget->raise();
        }
    } else {
        m_centralWidget = nullptr;
    }
}

/**
 * ChildWindowBase implementation
 */

ChildWindowBase::ChildWindowBase(QWidget *parent, Qt::WindowFlags flags, int minWidth, int minHeight)
    : FramelessWindowBase(parent, flags)
    , m_eventLoop(nullptr)
    , m_result(0) {
    setFrameless(true);
    FramelessWindowBase::setMinimumSize(minWidth, minHeight);
    setWindowModality(Qt::ApplicationModal);

    // Find parent FramelessWindowBase and connect mask signals
    FramelessWindowBase *fwParent = findParent<FramelessWindowBase>(this);
    if (!fwParent && parent) {
        fwParent = qobject_cast<FramelessWindowBase*>(parent->window());
    }
    
    if (fwParent) {
        connect(this, &FramelessWindowBase::showed, fwParent, [fwParent]() { fwParent->showMask(true); });
        connect(this, &FramelessWindowBase::closed, fwParent, [fwParent]() { fwParent->showMask(false); });
    }
}

int ChildWindowBase::exec() {
    if (m_eventLoop) {
        return m_result;
    }

    m_eventLoop = new QEventLoop(this);
    show();  // This will emit showed() signal via showEvent
    m_result = m_eventLoop->exec();
    delete m_eventLoop;
    m_eventLoop = nullptr;

    emit closed();
    return m_result;
}

void ChildWindowBase::closeEvent(QCloseEvent *event) {
    if (m_eventLoop) {
        m_eventLoop->exit(0);
    }
    FramelessWindowBase::closeEvent(event);
}