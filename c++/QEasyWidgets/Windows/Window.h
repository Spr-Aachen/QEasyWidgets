#ifndef QEASYWIDGETS_WINDOW_H
#define QEASYWIDGETS_WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGridLayout>
#include <QEventLoop>
#include <QCloseEvent>

#include "FramelessWindow/framelessWindow.h"


/**
 * Main window class with QMainWindow integration and frameless functionality
 */
class MainWindowBase : public FramelessWindowBase {
    Q_OBJECT

public:
    explicit MainWindowBase(
        QWidget *parent = nullptr, 
        Qt::WindowFlags flags = Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint,
        int minWidth = 1280,
        int minHeight = 720
    );
    ~MainWindowBase() override = default;

    void setCentralWidget(QWidget *centralWidget);
    QWidget *centralWidget() const {
        return m_centralWidget;
    }

    QGridLayout *centralLayout() const {
        return m_centralLayout;
    }

protected:
    void setupUI() override;

private:
    QGridLayout *m_centralLayout;
    QWidget *m_centralWidget;
};


/**
 * Child window that can be modal and has event loop execution
 */
class ChildWindowBase : public FramelessWindowBase {
    Q_OBJECT

public:
    explicit ChildWindowBase(
        QWidget *parent = nullptr,
        Qt::WindowFlags flags = Qt::Widget,
        int minWidth = 630,
        int minHeight = 420
    );
    ~ChildWindowBase() override = default;

    // Modal execution
    int exec();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QEventLoop *m_eventLoop;
    int m_result;
};


#endif // QEASYWIDGETS_WINDOW_H