#ifndef QEASYWIDGETS_FRAME_H
#define QEASYWIDGETS_FRAME_H

#include <QFrame>
#include "Widget.h"


namespace QEW {

/**
 * @brief Enhanced frame with theme support
 */
class FrameBase : public SizableWidget<QFrame>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit FrameBase(QWidget *parent = nullptr);
    ~FrameBase() override = default;

signals:
    void resized();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

} // namespace QEW

#endif // QEASYWIDGETS_FRAME_H
