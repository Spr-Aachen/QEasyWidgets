#include "ChatWidget.h"

#include <QHBoxLayout>

#include "../Common/Icon.h"


namespace QEW {

AvatarDisplay::AvatarDisplay(QWidget *parent)
    : QLabel(parent)
{
}

AvatarDisplay::AvatarDisplay(const QSize &size, const QPixmap &avatar, QWidget *parent)
    : QLabel(parent)
{
    setAvatar(avatar, size);
    setFixedSize(size);
}

void AvatarDisplay::setAvatar(const QPixmap &avatar, const QSize &size)
{
    setPixmap(avatar.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void AvatarDisplay::mouseDoubleClickEvent(QMouseEvent *event)
{
    QLabel::mouseDoubleClickEvent(event);
    emit clicked();
}

MessageDisplay::MessageDisplay(const QString &text, ChatRole role, QWidget *parent)
    : QLabel(text, parent)
    , m_role(role)
{
    init(role);
}

ChatRole MessageDisplay::role() const
{
    return m_role;
}

void MessageDisplay::init(ChatRole role)
{
    setWordWrap(true);
    setTextInteractionFlags(Qt::TextSelectableByMouse);

    QFont font("Microsoft YaHei", 12);
    setFont(font);

    // Set alignment and margins based on role
    if (role == ChatRole::User) {
        setAlignment(Qt::AlignRight);
        setContentsMargins(50, 5, 10, 5);
    } else {
        setAlignment(Qt::AlignLeft);
        setContentsMargins(10, 5, 50, 5);
    }
}

ChatWidgetBase::ChatWidgetBase(QWidget *parent)
    : QWidget(parent)
    , m_scrollArea(nullptr)
    , m_messageContainer(nullptr)
    , m_messageLayout(nullptr)
    , m_inputEdit(nullptr)
    , m_sendButton(nullptr)
    , m_inputLayout(nullptr)
    , m_mainLayout(nullptr)
{
    init();
}

void ChatWidgetBase::addMessage(const QString &text, ChatRole role, const QPixmap &avatar)
{
    QWidget *messageWidget = new QWidget();
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);
    messageLayout->setContentsMargins(0, 0, 0, 0);

    if (role == ChatRole::User) {
        // User message: message on the right, avatar on the left if provided
        MessageDisplay *message = new MessageDisplay(text, role);
        messageLayout->addStretch();
        messageLayout->addWidget(message);

        if (!avatar.isNull()) {
            AvatarDisplay *avatarLabel = new AvatarDisplay(QSize(30, 30), avatar);
            messageLayout->addWidget(avatarLabel);
        }
    } else {
        // Assistant/System message: avatar on the left, message on the right
        if (!avatar.isNull()) {
            AvatarDisplay *avatarLabel = new AvatarDisplay(QSize(30, 30), avatar);
            messageLayout->addWidget(avatarLabel);
        }

        MessageDisplay *message = new MessageDisplay(text, role);
        messageLayout->addWidget(message);
        messageLayout->addStretch();
    }

    m_messageLayout->addWidget(messageWidget);

    // Scroll to bottom
    QTimer::singleShot(0, this, [this]() {
        QScrollBar *scrollBar = m_scrollArea->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });
}

void ChatWidgetBase::clearMessages()
{
    // Clear all message widgets
    while (QLayoutItem *item = m_messageLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void ChatWidgetBase::init()
{
    setupUI();
}

void ChatWidgetBase::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Message area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_messageContainer = new QWidget();
    m_messageLayout = new QVBoxLayout(m_messageContainer);
    m_messageLayout->setContentsMargins(10, 10, 10, 10);
    m_messageLayout->setSpacing(10);
    m_messageLayout->addStretch();

    m_scrollArea->setWidget(m_messageContainer);
    m_mainLayout->addWidget(m_scrollArea);

    // Input area
    QWidget *inputWidget = new QWidget();
    m_inputLayout = new QHBoxLayout(inputWidget);
    m_inputLayout->setContentsMargins(10, 10, 10, 10);
    m_inputLayout->setSpacing(10);

    m_inputEdit = new QTextEdit();
    m_inputEdit->setMaximumHeight(80);
    m_inputEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_inputEdit->setPlaceholderText("Type your message...");

    m_sendButton = new ButtonBase();
    m_sendButton->setIcon(IconBase::Send);
    m_sendButton->setFixedSize(40, 40);

    m_inputLayout->addWidget(m_inputEdit);
    m_inputLayout->addWidget(m_sendButton);

    m_mainLayout->addWidget(inputWidget);

    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidgetBase::onSendClicked);
}

void ChatWidgetBase::onSendClicked()
{
    QString text = m_inputEdit->toPlainText().trimmed();
    if (!text.isEmpty()) {
        emit messageSent(text);
        m_inputEdit->clear();
    }
}

} // namespace QEW
