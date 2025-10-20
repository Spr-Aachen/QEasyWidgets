#ifndef QEASYWIDGETS_PROGRESSBAR_H
#define QEASYWIDGETS_PROGRESSBAR_H

#include <QProgressBar>
#include "Widget.h"


namespace QEW {

/**
 * @brief Enhanced progress bar with theme support
 */
class ProgressBarBase : public SizableWidget<QProgressBar>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit ProgressBarBase(QWidget *parent = nullptr);
    ~ProgressBarBase() override = default;

    void setBorderless(bool borderless);
    void setTransparent(bool transparent);

    void clearDefaultStyleSheet();

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_PROGRESSBAR_H
