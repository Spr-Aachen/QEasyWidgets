#include "Frame.h"


/**
 * FrameBase implementation
 */

FrameBase::FrameBase(QWidget *parent)
    : SizableWidget<QFrame>(parent) {
}

void FrameBase::resizeEvent(QResizeEvent *event) {
    emit resized();
    QFrame::resizeEvent(event);
}