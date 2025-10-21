#ifndef QEASYWIDGETS_DIALOG_H
#define QEASYWIDGETS_DIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEventLoop>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QStyle>
#include <QTextBrowser>
#include <QLineEdit>
#include <QIcon>
#include <QPixmap>

#include "FramelessWindow/framelessWindow.h"


/**
 * Base dialog class with theme support and frameless functionality
 */
class DialogBase : public FramelessWindowBase {
    Q_OBJECT

public:
    explicit DialogBase(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Dialog);
    ~DialogBase() override = default;

    // Modal execution
    virtual int exec();
    void accept();
    void reject();

protected:
    void setupUI() override;
    void closeEvent(QCloseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    QVBoxLayout *m_mainLayout;

    QEventLoop *m_eventLoop;
    int m_result;

signals:
    void accepted();
    void rejected();
    void finished(int result);
};


/**
 * Message box with standard buttons and icons
 */
class MessageBoxBase : public DialogBase {
    Q_OBJECT

public:
    explicit MessageBoxBase(QWidget *parent = nullptr, int minWidth = 360, int minHeight = 210);
    ~MessageBoxBase() override = default;

    // Standard buttons
    void setStandardButtons(QMessageBox::StandardButtons buttons);
    QMessageBox::StandardButton clickedButton() const {
        return m_clickedButton;
    }

    // Icon support
    void setIcon(QMessageBox::Icon icon);
    void setIcon(const QIcon &icon);
    void setIcon(const QPixmap &pixmap);
    void setWindowIcon(QMessageBox::Icon icon);

    // Text
    void setText(const QString &text, float textSize = 11.1f, int textWeight = 420);
    
    // Detailed text
    void setDetailedText(const QString &text);
    QTextBrowser *detailedTextBrowser();

    // Input dialog functionality
    QPair<QString, bool> getText(
        const QString &title,
        const QString &label,
        QLineEdit::EchoMode echo = QLineEdit::Normal,
        const QString &text = QString(),
        Qt::WindowFlags flags = Qt::Dialog,
        Qt::InputMethodHints inputMethodHints = Qt::ImhNone
    );

    // Static convenience method
    static QMessageBox::StandardButton pop(
        QWidget *windowToMask = nullptr,
        QMessageBox::Icon messageType = QMessageBox::Information,
        const QString &windowTitle = QString(),
        const QString &text = QString(),
        const QString &detailedText = QString(),
        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
        const QMap<QMessageBox::StandardButton, std::function<void()>> &buttonEvents = QMap<QMessageBox::StandardButton, std::function<void()>>()
    );

    int exec() override;

protected:
    QLabel *m_iconLabel;
    QLabel *m_textLabel;
    QDialogButtonBox *m_buttonBox;
    QTextBrowser *m_detailedTextBrowser;
    QMessageBox::StandardButton m_clickedButton;

private slots:
    void onButtonClicked(QAbstractButton *button);
};


/**
 * Input dialog for getting text input from user
 */
class InputDialogBase : public MessageBoxBase {
    Q_OBJECT

public:
    explicit InputDialogBase(QWidget *parent = nullptr, int minWidth = 420, int minHeight = 210);
    ~InputDialogBase() override = default;

    // Static convenience method
    static QPair<QString, bool> getText(
        QWidget *parent,
        const QString &title,
        const QString &label,
        QLineEdit::EchoMode echo = QLineEdit::Normal,
        const QString &text = QString(),
        Qt::WindowFlags flags = Qt::Dialog,
        Qt::InputMethodHints inputMethodHints = Qt::ImhNone
    );
};


#endif // QEASYWIDGETS_DIALOG_H