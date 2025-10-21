#ifndef QEASYWIDGETS_DOCKWIDGET_H
#define QEASYWIDGETS_DOCKWIDGET_H

#include <QDockWidget>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include "Button.h"
#include "Widget.h"


namespace QEW {

/**
 * @brief Custom title bar for dock widgets
 */
class DockTitleBar : public QWidget
{
    Q_OBJECT

public:
    static constexpr int DEFAULT_TITLE_BAR_HEIGHT = 30;

    explicit DockTitleBar(QDockWidget *parent = nullptr);
    ~DockTitleBar() override = default;

    ButtonBase* closeButton() const { return m_closeButton; }
    ButtonBase* floatButton() const { return m_floatButton; }
    ButtonBase* minimizeButton() const { return m_minimizeButton; }
    QLabel* titleLabel() const { return m_titleLabel; }

    void setTitle(const QString &title);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void onCloseClicked();
    void onFloatClicked();
    void onMinimizeClicked();

private:
    void init();
    void setupButtons();

    QDockWidget *m_dockWidget;
    QHBoxLayout *m_layout;
    QLabel *m_titleLabel;
    ButtonBase *m_closeButton;
    ButtonBase *m_floatButton;
    ButtonBase *m_minimizeButton;
};

/**
 * @brief Enhanced dock widget with theme support and custom title bar
 */
class DockWidgetBase : public SizableWidget<QDockWidget>
{
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

    DockTitleBar* customTitleBar() const { return m_customTitleBar; }

private:
    void init();

    DockTitleBar *m_customTitleBar;
};

} // namespace QEW

#endif // QEASYWIDGETS_DOCKWIDGET_H
