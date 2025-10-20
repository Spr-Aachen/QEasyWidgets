#include "DockWidget.h"

#include <QMouseEvent>

#include "../Common/StyleSheet.h"
#include "../Common/Icon.h"


namespace QEW {

// DockTitleBar implementation
DockTitleBar::DockTitleBar(QDockWidget *parent)
    : QWidget(parent)
    , m_dockWidget(parent)
    , m_layout(nullptr)
    , m_titleLabel(nullptr)
    , m_closeButton(nullptr)
    , m_floatButton(nullptr)
    , m_minimizeButton(nullptr)
{
    init();
}

void DockTitleBar::init()
{
    setFixedHeight(DEFAULT_TITLE_BAR_HEIGHT / 2);

    m_layout = new QHBoxLayout(this);
    m_layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    // Add title label
    m_titleLabel = new QLabel(this);
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->addWidget(m_titleLabel);

    m_layout->addStretch(1);

    setupButtons();

    StyleSheetBase::apply(this, StyleSheetBase::Bar);
}

void DockTitleBar::setupButtons()
{
    QSize iconSize(DEFAULT_TITLE_BAR_HEIGHT / 4, DEFAULT_TITLE_BAR_HEIGHT / 4);

    // Minimize button
    m_minimizeButton = new ButtonBase(this);
    m_minimizeButton->setBorderless(true);
    m_minimizeButton->setTransparent(true);
    m_minimizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_minimizeButton->setHoverBackgroundColor(QColor(123, 123, 123, 123));
    m_minimizeButton->setIcon(IconBase::Dash);
    m_minimizeButton->setIconSize(iconSize);
    m_minimizeButton->setCursor(Qt::PointingHandCursor);
    connect(m_minimizeButton, &QPushButton::clicked, this, &DockTitleBar::onMinimizeClicked);
    m_layout->addWidget(m_minimizeButton, 0, Qt::AlignRight);

    // Float button
    m_floatButton = new ButtonBase(this);
    m_floatButton->setBorderless(true);
    m_floatButton->setTransparent(true);
    m_floatButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_floatButton->setHoverBackgroundColor(QColor(123, 123, 123, 123));
    m_floatButton->setIcon(IconBase::Window_FullScreen);
    m_floatButton->setIconSize(iconSize);
    m_floatButton->setCursor(Qt::PointingHandCursor);
    connect(m_floatButton, &QPushButton::clicked, this, &DockTitleBar::onFloatClicked);
    m_layout->addWidget(m_floatButton, 0, Qt::AlignRight);

    // Close button
    m_closeButton = new ButtonBase(this);
    m_closeButton->setBorderless(true);
    m_closeButton->setTransparent(true);
    m_closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_closeButton->setHoverBackgroundColor(QColor(210, 123, 123, 210));
    m_closeButton->setIcon(IconBase::X);
    m_closeButton->setIconSize(iconSize);
    m_closeButton->setCursor(Qt::PointingHandCursor);
    connect(m_closeButton, &QPushButton::clicked, this, &DockTitleBar::onCloseClicked);
    m_layout->addWidget(m_closeButton, 0, Qt::AlignRight);
}

void DockTitleBar::setTitle(const QString &title)
{
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void DockTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->position().y() >= 0 && event->position().y() < height() &&
        event->button() == Qt::LeftButton) {
        onFloatClicked();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void DockTitleBar::onCloseClicked()
{
    if (m_dockWidget) {
        m_dockWidget->close();
    }
}

void DockTitleBar::onFloatClicked()
{
    if (m_dockWidget) {
        m_dockWidget->setFloating(!m_dockWidget->isFloating());
        // Update icon based on floating state
        if (m_floatButton) {
            m_floatButton->setIcon(m_dockWidget->isFloating() ?
                IconBase::Window_Stack : IconBase::Window_FullScreen);
        }
    }
}

void DockTitleBar::onMinimizeClicked()
{
    if (m_dockWidget) {
        m_dockWidget->hide();
    }
}

// DockWidgetBase implementation

DockWidgetBase::DockWidgetBase(QWidget *parent)
    : SizableWidget<QDockWidget>(parent)
    , m_customTitleBar(nullptr)
{
    init();
}

DockWidgetBase::DockWidgetBase(const QString &title, QWidget *parent)
    : SizableWidget<QDockWidget>(title, parent)
    , m_customTitleBar(nullptr)
{
    init();
}

void DockWidgetBase::init()
{
    // Create and set custom title bar
    m_customTitleBar = new DockTitleBar(this);
    m_customTitleBar->setTitle(windowTitle());
    setTitleBarWidget(m_customTitleBar);

    // Connect title change signal
    connect(this, &QDockWidget::windowTitleChanged, m_customTitleBar, &DockTitleBar::setTitle);

    StyleSheetBase::apply(this, StyleSheetBase::DockWidget);
}

void DockWidgetBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
    style()->unpolish(this);
    style()->polish(this);
}

void DockWidgetBase::setTransparent(bool transparent)
{
    setProperty("isTransparent", transparent);
    style()->unpolish(this);
    style()->polish(this);
}

void DockWidgetBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
    setStyleSheet("");
}

} // namespace QEW
