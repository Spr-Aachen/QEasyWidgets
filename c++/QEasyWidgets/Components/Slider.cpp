#include "Slider.h"

#include "../Common/StyleSheet.h"


namespace QEW {

SliderBase::SliderBase(QWidget *parent)
    : SizableWidget<QSlider>(parent)
{
    init();
}

SliderBase::SliderBase(Qt::Orientation orientation, QWidget *parent)
    : SizableWidget<QSlider>(orientation, parent)
{
    init();
}

void SliderBase::init()
{
    StyleSheetBase::apply(this, StyleSheetBase::Slider);
}

void SliderBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

} // namespace QEW
