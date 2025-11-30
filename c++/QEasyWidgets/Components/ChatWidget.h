#ifndef QEASYWIDGETS_CHATWIDGET_H
#define QEASYWIDGETS_CHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QScrollArea>
#include <QString>
#include <QMap>
#include <QPainter>
#include <QSpacerItem>

#include "../Common/Config.h"
#include "Button.h"
#include "StatusWidget.h"
#include "ScrollArea.h"


/**
 * Avatar display widget
 */
class AvatarDisplay : public QLabel {
    Q_OBJECT

public:
    explicit AvatarDisplay(QWidget *parent = nullptr);
    explicit AvatarDisplay(const QSize &size, const QPixmap &avatar, QWidget *parent = nullptr);
    ~AvatarDisplay() override = default;

    void setAvatar(const QPixmap &avatar, const QSize &size = QSize(45, 45));

signals:
    void clicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};


/**
 * Message display widget
 */
class MessageDisplay : public QLabel {
    Q_OBJECT

public:
    explicit MessageDisplay(const QString &text, const ChatRole role, QWidget *parent = nullptr);
    ~MessageDisplay() override = default;

    ChatRole role() const;

    void setMarkdown(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void init(const ChatRole role);

    ChatRole m_role;
};


/**
 * Triangle widget for chat bubbles
 */
class Triangle : public QWidget {
    Q_OBJECT

public:
    explicit Triangle(const ChatRole role, QWidget *parent = nullptr);
    ~Triangle() override = default;

    ChatRole role() const {
        return m_role;
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ChatRole m_role;
};

/**
 * Layout for chat messages
 */
class MessageLayout : public QHBoxLayout {
    Q_OBJECT

public:
    explicit MessageLayout(const QString &message, const ChatRole role, 
                         StatusWidgetBase *status, QWidget *parent = nullptr);
    ~MessageLayout() override = default;

    AvatarDisplay *avatarDisplay() const {
        return m_avatarDisplay;
    }
    MessageDisplay *messageDisplay() const {
        return m_messageDisplay;
    }
    Triangle *triangle() const {
        return m_triangle;
    }
    StatusWidgetBase *statusWidget() const {
        return m_statusWidget;
    }

private:
    AvatarDisplay *m_avatarDisplay;
    MessageDisplay *m_messageDisplay;
    Triangle *m_triangle;
    StatusWidgetBase *m_statusWidget;
};

/**
 * Notice display widget
 */
class NoticeDisplay : public QLabel {
    Q_OBJECT

public:
    explicit NoticeDisplay(const QString &text, QWidget *parent = nullptr);
    ~NoticeDisplay() override = default;
};

/**
 * Chat widget with message display and input
 */
class ChatWidgetBase : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidgetBase(QWidget *parent = nullptr);
    ~ChatWidgetBase() override = default;

    void addMessage(const QString &text, const ChatRole role, const Status status = Status::Loading);
    void addNotice(const QString &notice);
    void clear();
    void setAvatar(const QPixmap &avatar, const ChatRole role);

signals:
    void messageSent(const QString &text);
    void avatarClicked(AvatarDisplay *avatar);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void update();
    void _removeAllWidgets(QLayout *layout, bool selfIgnored = true);
    void _storeAvatar(AvatarDisplay *avatarDisplay, const ChatRole role);

private slots:
    void onSendClicked();
    void onAvatarClicked();

private:
    void init();
    void setupUI();
    void clearDefaultStyleSheet();

    VerticalScrollArea *m_scrollArea;
    QWidget *m_scrollAreaContent;
    QVBoxLayout *m_scrollAreaContentLayout;
    QSpacerItem *m_scrollAreaContentSpacer;
    QTextEdit *m_inputEdit;
    ButtonBase *m_sendButton;
    QVBoxLayout *m_mainLayout;
    QMap<ChatRole, QPixmap> m_avatars;
};


#endif // QEASYWIDGETS_CHATWIDGET_H