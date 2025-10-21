#include "ChatWidget.h"

#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>
#include <QKeyEvent>
#include <QDebug>

#include "../Common/Icon.h"
#include "../Common/StyleSheet.h"


/**
 * AvatarDisplay implementation
 */

AvatarDisplay::AvatarDisplay(QWidget *parent)
    : QLabel(parent) {
}

AvatarDisplay::AvatarDisplay(const QSize &size, const QPixmap &avatar, QWidget *parent)
    : QLabel(parent) {
    setAvatar(avatar, size);
    setFixedSize(size);
}

void AvatarDisplay::setAvatar(const QPixmap &avatar, const QSize &size) {
    setPixmap(avatar.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void AvatarDisplay::mouseDoubleClickEvent(QMouseEvent *event) {
    QLabel::mouseDoubleClickEvent(event);
    emit clicked();
}

/**
 * MessageDisplay implementation
 */

MessageDisplay::MessageDisplay(const QString &text, const QString &role, QWidget *parent)
    : QLabel(text, parent)
    , m_role(role) {
    init(role);
}

QString MessageDisplay::role() const {
    return m_role;
}

void MessageDisplay::init(const QString &role) {
    m_role = role;
    
    QFont font("Microsoft YaHei", 12);
    setFont(font);
    setWordWrap(true);
    setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    setTextInteractionFlags(Qt::TextSelectableByMouse);
    setMarkdown(text());
}

void MessageDisplay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw background
    painter.setPen(Qt::NoPen);
    if (m_role == ChatRole::User) {
        painter.setBrush(QColor("#b2e281"));
    } else {
        painter.setBrush(Qt::white);
    }
    painter.drawRoundedRect(rect(), 6, 6);
    
    // Draw text
    QLabel::paintEvent(event);
}

void MessageDisplay::setMarkdown(const QString &text) {
    // Simple markdown support - in a real implementation, you might want to use a proper markdown renderer
    setTextFormat(Qt::MarkdownText);
    setText(text);
}

/**
 * Triangle implementation
 */
Triangle::Triangle(const QString &role, QWidget *parent)
    : QWidget(parent)
    , m_role(role) {
    setFixedSize(6, 45);
}

void Triangle::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    
    if (m_role == ChatRole::User) {
        painter.setBrush(QColor("#b2e281"));
        QPolygon triangle;
        triangle << QPoint(0, 20) << QPoint(0, 34) << QPoint(6, 27);
        painter.drawPolygon(triangle);
    } else {
        painter.setBrush(Qt::white);
        QPolygon triangle;
        triangle << QPoint(0, 27) << QPoint(6, 20) << QPoint(6, 34);
        painter.drawPolygon(triangle);
    }
}

/**
 * MessageLayout implementation
 */
MessageLayout::MessageLayout(const QString &message, const QString &role, 
                           StatusWidgetBase *status, QWidget *parent)
    : QHBoxLayout(parent)
    , m_avatarDisplay(nullptr)
    , m_messageDisplay(new MessageDisplay(message, role))
    , m_triangle(new Triangle(role))
    , m_statusWidget(status) {
    
    setSpacing(0);
    setContentsMargins(0, 0, 0, 0);
    
    // Setup layout based on role
    if (role == ChatRole::User) {
        addStretch();
        addWidget(m_messageDisplay);
        addWidget(m_triangle);
        
        if (m_statusWidget) {
            addWidget(m_statusWidget);
        }
        
        // Add avatar if available
        m_avatarDisplay = new AvatarDisplay(QSize(45, 45), QPixmap());
        connect(m_avatarDisplay, &AvatarDisplay::clicked, this, [this]() {
            emit avatarDisplay()->clicked();
        });
        addWidget(m_avatarDisplay);
    } else {
        // Add avatar if available
        m_avatarDisplay = new AvatarDisplay(QSize(45, 45), QPixmap());
        connect(m_avatarDisplay, &AvatarDisplay::clicked, this, [this]() {
            emit avatarDisplay()->clicked();
        });
        addWidget(m_avatarDisplay);
        
        addWidget(m_triangle);
        addWidget(m_messageDisplay);
        
        if (m_statusWidget) {
            addWidget(m_statusWidget);
        }
        
        addStretch();
    }
}

/**
 * NoticeDisplay implementation
 */
NoticeDisplay::NoticeDisplay(const QString &text, QWidget *parent)
    : QLabel(text, parent) {
    QFont font("Microsoft YaHei", 12);
    setFont(font);
    setWordWrap(true);
    setAlignment(Qt::AlignCenter);
    setTextInteractionFlags(Qt::TextSelectableByMouse);
}

/**
 * ChatWidgetBase implementation
 */
ChatWidgetBase::ChatWidgetBase(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(new VerticalScrollArea())
    , m_scrollAreaContent(new QWidget())
    , m_scrollAreaContentLayout(new QVBoxLayout(m_scrollAreaContent))
    , m_scrollAreaContentSpacer(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding))
    , m_inputEdit(new QTextEdit())
    , m_sendButton(new ButtonBase())
    , m_mainLayout(new QVBoxLayout(this)) {
    
    init();
}

void ChatWidgetBase::addMessage(const QString &text, const QString &role, const QString &status) {
    StatusWidgetBase *statusWidget = status.isEmpty() ? nullptr : new StatusWidgetBase(status);
    MessageLayout *messageLayout = new MessageLayout(text, role, statusWidget);
    
    // Store avatar if this is a new role
    if (m_avatars.contains(role)) {
        messageLayout->avatarDisplay()->setAvatar(m_avatars[role]);
    }
    
    // Connect avatar clicked signal
    if (messageLayout->avatarDisplay()) {
        connect(messageLayout->avatarDisplay(), &AvatarDisplay::clicked, 
                this, &ChatWidgetBase::onAvatarClicked);
    }
    
    // Create a widget to hold the layout
    QWidget *messageWidget = new QWidget();
    messageWidget->setLayout(messageLayout);
    
    // Insert before the spacer
    m_scrollAreaContentLayout->insertWidget(m_scrollAreaContentLayout->count() - 1, messageWidget);
    
    // Scroll to bottom
    QTimer::singleShot(0, this, [this]() {
        ScrollBar *scrollBar = m_scrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });
}

void ChatWidgetBase::addNotice(const QString &notice) {
    NoticeDisplay *noticeDisplay = new NoticeDisplay(notice);
    m_scrollAreaContentLayout->insertWidget(m_scrollAreaContentLayout->count() - 1, noticeDisplay);
}

void ChatWidgetBase::clear() {
    _removeAllWidgets(m_scrollAreaContentLayout, false);
    m_scrollAreaContentLayout->addItem(m_scrollAreaContentSpacer);
}

void ChatWidgetBase::setAvatar(const QPixmap &avatar, const QString &role) {
    m_avatars[role] = avatar;
    
    // Update all existing avatars with this role
    for (int i = 0; i < m_scrollAreaContentLayout->count() - 1; ++i) {
        QLayoutItem *item = m_scrollAreaContentLayout->itemAt(i);
        if (item && item->widget()) {
            MessageLayout *layout = qobject_cast<MessageLayout*>(item->widget()->layout());
            if (layout && layout->avatarDisplay() && layout->messageDisplay()->role() == role) {
                layout->avatarDisplay()->setAvatar(avatar);
            }
        }
    }
}

void ChatWidgetBase::update() {
    // Update layout and scroll to bottom
    m_scrollAreaContent->adjustSize();
    ScrollBar *scrollBar = m_scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidgetBase::_removeAllWidgets(QLayout *layout, bool selfIgnored) {
    if (!layout) return;
    
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
            child->widget()->deleteLater();
        } else if (child->layout()) {
            _removeAllWidgets(child->layout(), false);
        } else if (child->spacerItem() && !selfIgnored) {
            layout->removeItem(child);
            delete child;
        }
        delete child;
    }
}

void ChatWidgetBase::_storeAvatar(AvatarDisplay *avatarDisplay, const QString &role) {
    if (!avatarDisplay) {
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPixmap pm = avatarDisplay->pixmap(Qt::ReturnByValue);
    if (!pm.isNull()) {
        m_avatars[role] = pm;
    }
#else
    const QPixmap *pm = avatarDisplay->pixmap();
    if (pm && !pm->isNull()) {
        m_avatars[role] = *pm;
    }
#endif
}

void ChatWidgetBase::init() {
    setupUI();
    clearDefaultStyleSheet();
}

void ChatWidgetBase::setupUI() {
    // Main layout
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Scroll area setup
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_scrollAreaContent);
    
    // Scroll area content layout
    m_scrollAreaContentLayout->setSpacing(12);
    m_scrollAreaContentLayout->setContentsMargins(12, 12, 12, 12);
    m_scrollAreaContentLayout->addItem(m_scrollAreaContentSpacer);
    
    // Input area
    QWidget *inputWidget = new QWidget();
    QHBoxLayout *inputLayout = new QHBoxLayout(inputWidget);
    inputLayout->setContentsMargins(12, 12, 12, 12);
    inputLayout->setSpacing(12);
    
    m_inputEdit->setPlaceholderText("Type a message...");
    m_inputEdit->setMaximumHeight(80);
    
    m_sendButton->setIcon(IconBase::Send);
    m_sendButton->setFixedSize(40, 40);
    
    inputLayout->addWidget(m_inputEdit);
    inputLayout->addWidget(m_sendButton);
    
    // Add widgets to main layout
    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->addWidget(inputWidget);
    
    // Connections
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidgetBase::onSendClicked);
    // Install an event filter on the QTextEdit to handle Enter (without Shift) as send.
    m_inputEdit->installEventFilter(this);
}

bool ChatWidgetBase::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_inputEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            // Send on Enter, but allow newline when Shift is held.
            if (!(keyEvent->modifiers() & Qt::ShiftModifier)) {
                onSendClicked();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ChatWidgetBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void ChatWidgetBase::onSendClicked() {
    QString text = m_inputEdit->toPlainText().trimmed();
    if (!text.isEmpty()) {
        emit messageSent(text);
        m_inputEdit->clear();
    }
}

void ChatWidgetBase::onAvatarClicked() {
    AvatarDisplay *avatar = qobject_cast<AvatarDisplay*>(sender());
    if (avatar) {
        emit avatarClicked(avatar);
    }
}