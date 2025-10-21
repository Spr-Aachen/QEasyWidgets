#ifndef QEASYWIDGETS_BAR_H
#define QEASYWIDGETS_BAR_H

#include <QWidget>
#include <QHBoxLayout>

#include "Button.h"
#include "Label.h"


/**
 * Forward declaration
 */
class QMouseEvent;
class QLabel;


/**
 * Title bar with window control buttons
 */
class TitleBarBase : public QWidget {
    Q_OBJECT

public:
    static const int DEFAULT_TITLE_BAR_HEIGHT = 30;

    explicit TitleBarBase(QWidget *parent = nullptr);
    ~TitleBarBase() override = default;

    void setTitle(const QString &text);
    QString title() const;

    void clearDefaultStyleSheet();

    // Button accessors
    ButtonBase *closeButton() const {
        return m_closeButton;
    }
    ButtonBase *maximizeButton() const {
        return m_maximizeButton;
    }
    ButtonBase *minimizeButton() const {
        return m_minimizeButton;
    }

public slots:
    void closeWindow();
    void maximizeWindow();
    void minimizeWindow();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void init();
    void setupButtons();

    QWidget *m_window;
    QLabel *m_titleLabel;
    ButtonBase *m_closeButton;
    ButtonBase *m_maximizeButton;
    ButtonBase *m_minimizeButton;
    QHBoxLayout *m_layout;
};


#endif // QEASYWIDGETS_BAR_H