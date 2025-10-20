#ifndef QEASYWIDGETS_SLIDER_H
#define QEASYWIDGETS_SLIDER_H

#include <QSlider>
#include "Widget.h"


namespace QEW {

/**
 * @brief Enhanced slider with theme support
 */
class SliderBase : public SizableWidget<QSlider>
{
    Q_OBJECT
    Q_PROPERTY(int currentWidth READ currentWidth WRITE setCurrentWidth)
    Q_PROPERTY(int currentHeight READ currentHeight WRITE setCurrentHeight)

public:
    explicit SliderBase(QWidget *parent = nullptr);
    explicit SliderBase(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~SliderBase() override = default;

    void clearDefaultStyleSheet();

signals:
    void valueChanged(int value);

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_SLIDER_H
