#ifndef QEASYWIDGETS_DOCKWIDGET_H
#define QEASYWIDGETS_DOCKWIDGET_H

#include <QDockWidget>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include "Bar.h"
#include "Button.h"
#include "Widget.h"


/**
 * Custom title bar for dock widgets
 */
class DockTitleBar : public TitleBarBase {
    Q_OBJECT

public:
    explicit DockTitleBar(QDockWidget *parent = nullptr);
    ~DockTitleBar() override = default;

    ButtonBase* floatButton() const {
        return m_floatButton;
    }

private slots:
    void onFloatClicked();

private:
    void setupButtons();

    QDockWidget *m_dockWidget;
    ButtonBase *m_floatButton;
};


/**
 * Enhanced dock widget with theme support and custom title bar
 */
class DockWidgetBase : public SizableWidget<QDockWidget> {
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit DockWidgetBase(QWidget *parent = nullptr);
    explicit DockWidgetBase(const QString &title, QWidget *parent = nullptr);
    ~DockWidgetBase() override = default;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);
    void clearDefaultStyleSheet();

    DockTitleBar* customTitleBar() const {
        return m_customTitleBar;
    }

private:
    void init();

    DockTitleBar *m_customTitleBar;
};


#endif // QEASYWIDGETS_DOCKWIDGET_H