#include "ToolBox.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QStyle>

#include "../Common/StyleSheet.h"
#include "../Common/Theme.h"
#include "../Common/QFunctions.h"


/**
 * Folder implementation
 */

Folder::Folder(QWidget *parent)
    : QLabel(parent) {
    init();
}

void Folder::init() {
    installEventFilter(this);
    setFixedHeight(FOLDER_HEIGHT);

    // Set font size
    QFont font = this->font();
    font.setPointSize(static_cast<int>(FOLDER_HEIGHT * 0.45));
    setFont(font);

    // Create folder button
    m_folderButton = new RotateButton(this);
    m_folderButton->setFixedSize(FOLDER_HEIGHT - FOLDER_MARGIN, FOLDER_HEIGHT - FOLDER_MARGIN);
    connect(m_folderButton, &RotateButton::clicked, this, &Folder::clicked);

    // Setup hover color
    QColor defaultHoverColor = getThemeColor(ThemeColor::Default);
    defaultHoverColor.setAlpha(12);
    m_hoverColor = defaultHoverColor;

    // Create layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(FOLDER_MARGIN, FOLDER_MARGIN, FOLDER_MARGIN, FOLDER_MARGIN);
    layout->setSpacing(0);
    layout->addWidget(m_folderButton, 0, Qt::AlignRight);
}

void Folder::enterEvent(QEnterEvent *event) {
    m_isEntered = true;
    update();
    QLabel::enterEvent(event);
}

void Folder::leaveEvent(QEvent *event) {
    m_isEntered = false;
    update();
    QLabel::leaveEvent(event);
}

void Folder::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_folderButton->click();
    }
    QLabel::mousePressEvent(event);
}

void Folder::paintEvent(QPaintEvent *event) {
    QLabel::paintEvent(event);
    if (m_isEntered) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), m_hoverColor);
    }
}

bool Folder::eventFilter(QObject *watched, QEvent *event) {
    if (watched == this && event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_folderButton->click();
        }
    }
    return QLabel::eventFilter(watched, event);
}


/**
 * ToolPage implementation
 */

ToolPage::ToolPage(QWidget *parent)
    : WidgetBase(parent) {
    init();
}

void ToolPage::init() {
    m_isExpanded = true;

    // Create folder header
    m_folder = new Folder(this);
    connect(m_folder, &Folder::clicked, this, [this]() {
        if (m_isExpanded) {
            collapse();
        } else {
            expand();
        }
    });

    // Create content widget
    m_widget = new WidgetBase();
    m_widget->setAttribute(Qt::WA_StyledBackground);
    QGridLayout *widgetLayout = new QGridLayout();
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    widgetLayout->setSpacing(0);
    m_widget->setLayout(widgetLayout);
    connect(m_widget, &WidgetBase::resized, this, [this]() {
        updateHeight();
    });

    // Main layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_folder);
    layout->addWidget(m_widget);
}

void ToolPage::addWidget(QWidget *widget) {
    if (m_widget && m_widget->layout()) {
        m_widget->layout()->addWidget(widget);
        updateHeight(widget);
    }
}

void ToolPage::setText(const QString &text) {
    if (m_folder) {
        m_folder->setText(text);
    }
}

QString ToolPage::text() const {
    return m_folder ? m_folder->text() : QString();
}

void ToolPage::expand() {
    if (!m_widget) return;
    
    // Animate widget to expanded size
    int targetHeight = m_widget->sizeHint().height();
    QPropertyAnimation *animation = new QPropertyAnimation(m_widget, "minimumHeight", this);
    animation->setDuration(300);
    animation->setStartValue(0);
    animation->setEndValue(targetHeight);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    if (m_widget->layout()) {
        m_widget->layout()->setSizeConstraint(QLayout::SetMinimumSize);
    }
    m_isExpanded = true;
}

void ToolPage::collapse() {
    if (!m_widget) return;
    
    // Animate widget to collapsed size
    QPropertyAnimation *animation = new QPropertyAnimation(m_widget, "minimumHeight", this);
    animation->setDuration(300);
    animation->setStartValue(m_widget->height());
    animation->setEndValue(0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    if (m_widget->layout()) {
        m_widget->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
    }
    m_isExpanded = false;
}

void ToolPage::updateHeight(QWidget *addedWidget) {
    if (!m_folder || !m_widget) return;

    int buttonHeight = m_folder->height();
    int layoutSpacing = layout() ? layout()->spacing() : 0;
    
    QMargins margins = m_widget->layout() ? m_widget->layout()->contentsMargins() : QMargins();
    int widgetLayoutMargins = margins.top() + margins.bottom();
    
    int widgetHeight = addedWidget ? addedWidget->height() : m_widget->height();
    if (widgetHeight < 0) {
        widgetHeight = 0;
    }
    
    int adjustedHeight = buttonHeight + layoutSpacing + widgetHeight + widgetLayoutMargins;
    setFixedHeight(adjustedHeight);
}

void ToolPage::resizeEvent(QResizeEvent *event) {
    WidgetBase::resizeEvent(event);
    if (m_folder) {
        m_folder->setFixedWidth(width());
    }
    if (m_widget) {
        m_widget->setFixedWidth(width());
    }
}


/**
 * ToolBoxBase implementation
 */

ToolBoxBase::ToolBoxBase(QWidget *parent)
    : FrameBase(parent) {
    init();
}

void ToolBoxBase::init() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    StyleSheetBase::apply(this, StyleSheetBase::ToolBox);
}

void ToolBoxBase::addItem(QWidget *widget, const QString &text) {
    // Check if page with this text already exists
    for (ToolPage *toolPage : m_toolPages) {
        if (toolPage->text() == text) {
            toolPage->addWidget(widget);
            updateHeight();
            return;
        }
    }

    // Create new page
    ToolPage *toolPage = new ToolPage(this);
    toolPage->setText(text);
    toolPage->addWidget(widget);

    // Connect resize signal
    connect(toolPage, &ToolPage::resized, this, [this]() {
        updateHeight();
    });

    layout()->addWidget(toolPage);
    m_toolPages.append(toolPage);

    if (m_currentIndex < 0) {
        m_currentIndex = 0;
    }

    updateHeight();
}

ToolPage *ToolBoxBase::widget(int index) const {
    if (index >= 0 && index < m_toolPages.count()) {
        return m_toolPages[index];
    }
    return nullptr;
}

void ToolBoxBase::setItemText(int index, const QString &text) {
    ToolPage *page = widget(index);
    if (page) {
        page->setText(text);
    }
}

void ToolBoxBase::setCurrentIndex(int index) {
    if (index < 0 || index >= m_toolPages.count()) {
        return;
    }

    // Collapse all pages
    for (ToolPage *page : m_toolPages) {
        if (page->isExpanded()) {
            page->collapse();
        }
    }

    // Expand the specified page
    ToolPage *page = m_toolPages[index];
    if (!page->isExpanded()) {
        page->expand();
    }

    m_currentIndex = index;
}

int ToolBoxBase::currentIndex() const {
    return m_currentIndex;
}

int ToolBoxBase::indexOf(QWidget *widget) const {
    // If widget is a ToolPage, find its index directly
    if (auto *toolPage = qobject_cast<ToolPage *>(widget)) {
        return m_toolPages.indexOf(toolPage);
    }

    // Otherwise, try to find parent ToolPage
    QWidget *parent = widget->parentWidget();
    while (parent) {
        if (auto *toolPage = qobject_cast<ToolPage *>(parent)) {
            return m_toolPages.indexOf(toolPage);
        }
        parent = parent->parentWidget();
    }

    return -1;
}

void ToolBoxBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
    style()->polish(this);
}

void ToolBoxBase::setTransparent(bool transparent) {
    setProperty("isTransparent", transparent);
    style()->polish(this);
}

void ToolBoxBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void ToolBoxBase::updateHeight() {
    int layoutSpacing = 0;
    if (m_toolPages.count() > 1) {
        layoutSpacing = layout()->spacing() * (m_toolPages.count() - 1);
    }

    QMargins margins = layout()->contentsMargins();
    int layoutMargins = margins.top() + margins.bottom();
    int toolPagesHeight = layoutMargins;

    for (const ToolPage *toolPage : m_toolPages) {
        toolPagesHeight += toolPage->height();
    }

    int adjustedHeight = layoutSpacing + toolPagesHeight;
    setFixedHeight(adjustedHeight);
}