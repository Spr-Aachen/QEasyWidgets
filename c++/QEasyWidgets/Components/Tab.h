#ifndef QEASYWIDGETS_TAB_H
#define QEASYWIDGETS_TAB_H

#include <QTabWidget>
#include "Widget.h"


namespace QEW {

/**
 * @brief Enhanced tab widget with theme support
 */
class TabWidgetBase : public SizableWidget<QTabWidget>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit TabWidgetBase(QWidget *parent = nullptr);
    ~TabWidgetBase() override = default;

    void setBorderless(bool borderless);

    void clearDefaultStyleSheet();

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_TAB_H
