#ifndef QEASYWIDGETS_FRAMELESSWINDOW_H
#define QEASYWIDGETS_FRAMELESSWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QShowEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QRect>

#include "../../Common/Icon.h"
#include "../../Common/Theme.h"
#include "../../Components/Button.h"
#include "../../Components/Bar.h"


enum WindowState {
    Normal,
    Maximized,
    Minimized
};


/**
 * Base class for frameless windows with platform-specific implementations
 */
class FramelessWindowBase : public QWidget {
    Q_OBJECT

public:
    explicit FramelessWindowBase(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    ~FramelessWindowBase() override = default;

    void setMinimumSize(int width, int height);
    void setFrameless(bool stretchable = true, bool dropShadowEffect = true);
    
    void setTitleBar(TitleBarBase *titleBar);

    QVBoxLayout *layout() const {
        return m_mainLayout;
    }

    void showMask(bool show, const QString &maskContent = QString());
    void setWindowState(WindowState state);
    WindowState windowState() const {
        return m_windowState;
    }

    TitleBarBase *titleBar() const {
        return m_titleBar;
    }

    int edgeSize() const {
        return m_edgeSize;
    }

    void setEdgeSize(int size) {
        m_edgeSize = size;
    }

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

    virtual void setupUI();
    virtual bool checkIfDraggable(const QPoint &pos) const;

signals:
    void showed();
    void closed();
    void rectChanged(const QRect &rect);

private slots:
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();

private:
    // Theme animation helpers (composition instead of multiple QObject inheritance)
    BackgroundColorAnimationBase *m_backgroundAnimation;
    TextColorAnimationBase *m_textAnimation;

    void updateWindowGeometry();
    void restoreWindow();
    void maximizeWindow();

    TitleBarBase *m_titleBar;
    QVBoxLayout *m_mainLayout;
    QLabel *m_maskWidget;

    WindowState m_windowState;
    QRect m_normalGeometry;

    bool m_stretchable;
    bool m_dragging;
    QPoint m_dragStartPosition;

    int m_edgeSize;  // Border edge size for resize detection
    bool m_resizing;
    int m_resizeDirection;
};


#endif // QEASYWIDGETS_FRAMELESSWINDOW_H