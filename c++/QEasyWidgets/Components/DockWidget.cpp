#include "DockWidget.h"

#include <QMouseEvent>
#include <QStyle>

#include "Bar.h"
#include "../Common/Icon.h"
#include "../Common/StyleSheet.h"


/**
 * DockTitleBar implementation
 */

DockTitleBar::DockTitleBar(QDockWidget *parent)
    : TitleBarBase(parent)
    , m_dockWidget(parent)
    , m_floatButton(nullptr) {
    setFixedHeight(DEFAULT_TITLE_BAR_HEIGHT / 2);
    
    // Set up the float button
    setupButtons();
    
    // Set up button icons and appearance
    QSize iconSize(DEFAULT_TITLE_BAR_HEIGHT / 4, DEFAULT_TITLE_BAR_HEIGHT / 4);
    
    // Configure close button from base class
    closeButton()->setBorderless(true);
    closeButton()->setTransparent(true);
    closeButton()->setHoverBackgroundColor(QColor(210, 123, 123, 210));
    closeButton()->setIcon(IconBase::X);
    closeButton()->setIconSize(iconSize);
    closeButton()->setCursor(Qt::PointingHandCursor);
    
    // Configure minimize button from base class
    minimizeButton()->setBorderless(true);
    minimizeButton()->setTransparent(true);
    minimizeButton()->setHoverBackgroundColor(QColor(123, 123, 123, 123));
    minimizeButton()->setIcon(IconBase::Dash);
    minimizeButton()->setIconSize(iconSize);
    minimizeButton()->setCursor(Qt::PointingHandCursor);
    
    // Configure float button
    m_floatButton->setIcon(IconBase::FullScreen_Exit);
    m_floatButton->setIconSize(iconSize);
    
    // Remove the maximize button as it's not needed for dock widgets
    maximizeButton()->deleteLater();
}

void DockTitleBar::setupButtons() {
    // Create float button
    m_floatButton = new ButtonBase(this);
    m_floatButton->setBorderless(true);
    m_floatButton->setTransparent(true);
    m_floatButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_floatButton->setHoverBackgroundColor(QColor(123, 123, 123, 123));
    m_floatButton->setCursor(Qt::PointingHandCursor);
    
    // Add the float button to the layout (after minimize button, before close button)
    QHBoxLayout* barLayout = qobject_cast<QHBoxLayout*>(layout());
    if (barLayout) {
        // Remove the close button temporarily
        QWidget* closeBtn = closeButton();
        barLayout->removeWidget(closeBtn);
        
        // Add float button
        barLayout->addWidget(m_floatButton, 0, Qt::AlignRight);
        
        // Add close button back
        barLayout->addWidget(closeBtn, 0, Qt::AlignRight);
    }
    
    // Connect float button signal
    connect(m_floatButton, &QPushButton::clicked, this, &DockTitleBar::onFloatClicked);
}

/**
 * DockWidgetBase implementation
 */

DockWidgetBase::DockWidgetBase(QWidget *parent)
    : SizableWidget<QDockWidget>(parent)
    , m_customTitleBar(nullptr) {
    init();
}

DockWidgetBase::DockWidgetBase(const QString &title, QWidget *parent)
    : SizableWidget<QDockWidget>(title, parent)
    , m_customTitleBar(nullptr) {
    init();
}

void DockWidgetBase::init() {
    // Create and set custom title bar
    m_customTitleBar = new DockTitleBar(this);
    m_customTitleBar->setTitle(windowTitle());
    setTitleBarWidget(m_customTitleBar);

    // Connect title change signal
    connect(this, &QDockWidget::windowTitleChanged, m_customTitleBar, &DockTitleBar::setTitle);

    StyleSheetBase::apply(this, StyleSheetBase::DockWidget);
}

void DockWidgetBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
    style()->unpolish(this);
    style()->polish(this);
}

void DockWidgetBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
    style()->unpolish(this);
    style()->polish(this);
}

void DockWidgetBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}