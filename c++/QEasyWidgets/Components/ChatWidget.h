#ifndef QEASYWIDGETS_CHATWIDGET_H
#define QEASYWIDGETS_CHATWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QScrollArea>

#include "Button.h"


namespace QEW {

enum class ChatRole {
    User,
    Assistant,
    System
};

/**
 * @brief Avatar display widget
 */
class AvatarDisplay : public QLabel
{
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
 * @brief Message display widget
 */
class MessageDisplay : public QLabel
{
    Q_OBJECT

public:
    explicit MessageDisplay(const QString &text, ChatRole role, QWidget *parent = nullptr);
    ~MessageDisplay() override = default;

    ChatRole role() const;

private:
    void init(ChatRole role);

    ChatRole m_role;
};

/**
 * @brief Chat widget with message display and input
 */
class ChatWidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidgetBase(QWidget *parent = nullptr);
    ~ChatWidgetBase() override = default;

    void addMessage(const QString &text, ChatRole role, const QPixmap &avatar = QPixmap());
    void clearMessages();

signals:
    void messageSent(const QString &text);

private slots:
    void onSendClicked();

private:
    void init();
    void setupUI();

    QScrollArea *m_scrollArea;
    QWidget *m_messageContainer;
    QVBoxLayout *m_messageLayout;
    QTextEdit *m_inputEdit;
    ButtonBase *m_sendButton;
    QHBoxLayout *m_inputLayout;
    QVBoxLayout *m_mainLayout;
};

} // namespace QEW

#endif // QEASYWIDGETS_CHATWIDGET_H
