#ifndef QEASYWIDGETS_FRAMELESSWINDOW_H
#define QEASYWIDGETS_FRAMELESSWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>

#include "../../Common/Icon.h"
#include "../../Common/Theme.h"
#include "../../Components/Button.h"
#include "../../Components/Bar.h"

namespace QEW {

enum WindowState {
    Normal,
    Maximized,
    Minimized
};

// Use TitleBarBase from Components/Bar.h instead of defining TitleBar here

/**
 * @brief Base class for frameless windows
 */
class FramelessWindowBase : public QWidget
{
    Q_OBJECT

public:
    explicit FramelessWindowBase(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    ~FramelessWindowBase() override = default;

    void setMinimumSize(int width, int height);
    void setFrameless(bool stretchable = true);

    QVBoxLayout *layout() const { return m_mainLayout; }

    void showMask(bool show);
    void setWindowState(WindowState state);
    WindowState windowState() const { return m_windowState; }

    TitleBarBase *titleBar() const { return m_titleBar; }

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    virtual void setupUI();

signals:
    void showed();
    void closed();

private slots:
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();

private:
    void updateWindowGeometry();
    void restoreWindow();
    void maximizeWindow();

    TitleBarBase *m_titleBar;
    QVBoxLayout *m_mainLayout;

    WindowState m_windowState;
    QRect m_normalGeometry;

    bool m_stretchable;
    bool m_dragging;
    QPoint m_dragStartPosition;

    // For Windows platform
    int m_borderWidth;
    bool m_resizing;
    int m_resizeDirection;

    QWidget *m_maskWidget;
};

} // namespace QEW

#endif // QEASYWIDGETS_FRAMELESSWINDOW_H
