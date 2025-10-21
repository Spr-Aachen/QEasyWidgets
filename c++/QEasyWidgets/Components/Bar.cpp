#include "Bar.h"

#include <QMainWindow>
#include <QDialog>
#include <QDockWidget>
#include <QSizePolicy>
#include <QMouseEvent>

#include "../Common/Icon.h"
#include "../Common/StyleSheet.h"


/**
 * TitleBarBase implementation
 */

TitleBarBase::TitleBarBase(QWidget *parent)
    : QWidget(parent)
    , m_window(nullptr)
    , m_titleLabel(nullptr)
    , m_closeButton(nullptr)
    , m_maximizeButton(nullptr)
    , m_minimizeButton(nullptr)
    , m_layout(nullptr) {
    init();
}

void TitleBarBase::init() {
    // Find the parent window
    QWidget *parent = parentWidget();
    while (parent) {
        if (qobject_cast<QMainWindow*>(parent) ||
            qobject_cast<QDialog*>(parent) ||
            qobject_cast<QDockWidget*>(parent)) {
            m_window = parent;
            break;
        }
        parent = parent->parentWidget();
    }
    if (!m_window) {
        m_window = window();
    }

    setFixedHeight(DEFAULT_TITLE_BAR_HEIGHT);
    setGeometry(0, 0, m_window->width(), height());

    m_layout = new QHBoxLayout(this);
    m_layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addStretch(1);

    setupButtons();

    StyleSheetBase::apply(this, StyleSheetBase::Bar);
}

void TitleBarBase::setupButtons() {
    QSize iconSize(DEFAULT_TITLE_BAR_HEIGHT / 2, DEFAULT_TITLE_BAR_HEIGHT / 2);

    // Close button
    m_closeButton = new ButtonBase(this);
    m_closeButton->setBorderless(true);
    m_closeButton->setTransparent(true);
    m_closeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_closeButton->setHoverBackgroundColor(QColor(210, 123, 123, 210));
    m_closeButton->setIcon(IconBase::X);
    m_closeButton->setCursor(Qt::PointingHandCursor);
    m_closeButton->setIconSize(iconSize);
    connect(m_closeButton, &QPushButton::clicked, this, &TitleBarBase::closeWindow);
    m_layout->addWidget(m_closeButton, 0, Qt::AlignRight);

    // Maximize button
    m_maximizeButton = new ButtonBase(this);
    m_maximizeButton->setBorderless(true);
    m_maximizeButton->setTransparent(true);
    m_maximizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_maximizeButton->setHoverBackgroundColor(QColor(123, 123, 123, 123));
    m_maximizeButton->setIcon(IconBase::FullScreen);
    m_maximizeButton->setCursor(Qt::PointingHandCursor);
    m_maximizeButton->setIconSize(iconSize);
    connect(m_maximizeButton, &QPushButton::clicked, this, &TitleBarBase::maximizeWindow);
    m_layout->insertWidget(m_layout->indexOf(m_closeButton), m_maximizeButton, 0, Qt::AlignRight);

    // Minimize button
    m_minimizeButton = new ButtonBase(this);
    m_minimizeButton->setBorderless(true);
    m_minimizeButton->setTransparent(true);
    m_minimizeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_minimizeButton->setHoverBackgroundColor(QColor(123, 123, 123, 123));
    m_minimizeButton->setIcon(IconBase::Dash);
    m_minimizeButton->setCursor(Qt::PointingHandCursor);
    m_minimizeButton->setIconSize(iconSize);
    connect(m_minimizeButton, &QPushButton::clicked, this, &TitleBarBase::minimizeWindow);
    m_layout->insertWidget(m_layout->indexOf(m_maximizeButton), m_minimizeButton, 0, Qt::AlignRight);
}

void TitleBarBase::setTitle(const QString &text) {
    if (!m_titleLabel) {
        m_titleLabel = new LabelBase(this);
        m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_layout->insertWidget(0, m_titleLabel, 0, Qt::AlignLeft);
    }
    m_titleLabel->setText(text);
}

QString TitleBarBase::title() const {
    return m_titleLabel ? m_titleLabel->text() : QString();
}

void TitleBarBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void TitleBarBase::closeWindow() {
    if (m_window) {
        m_window->close();
    }
}

void TitleBarBase::maximizeWindow() {
    if (m_window) {
        if (m_window->isMaximized()) {
            m_window->showNormal();
            m_maximizeButton->setIcon(IconBase::FullScreen);
        } else {
            m_window->showMaximized();
            m_maximizeButton->setIcon(IconBase::FullScreen_Exit);
        }
    }
}

void TitleBarBase::minimizeWindow() {
    if (m_window) {
        m_window->showMinimized();
    }
}

void TitleBarBase::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->position().y() > 0 && event->position().y() < height() &&
        event->button() == Qt::LeftButton) {
        maximizeWindow();
    }
    QWidget::mouseDoubleClickEvent(event);
}