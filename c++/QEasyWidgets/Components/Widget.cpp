#include "Widget.h"


/**
 * WidgetBase implementation
 */

WidgetBase::WidgetBase(QWidget *parent)
    : SizableWidget<QWidget>(parent)
{
}

void WidgetBase::resizeEvent(QResizeEvent *event)
{
    emit resized();
    QWidget::resizeEvent(event);
}