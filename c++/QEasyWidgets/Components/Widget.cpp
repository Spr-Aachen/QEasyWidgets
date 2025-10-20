#include "Widget.h"


namespace QEW {

WidgetBase::WidgetBase(QWidget *parent)
    : SizableWidget<QWidget>(parent)
{
}

void WidgetBase::resizeEvent(QResizeEvent *event)
{
    emit resized();
    QWidget::resizeEvent(event);
}

} // namespace QEW
