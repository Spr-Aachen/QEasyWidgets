#include "SpinBox.h"

#include "../Common/StyleSheet.h"


namespace QEW {

SpinBoxBase::SpinBoxBase(QWidget *parent)
    : QSpinBox(parent)
{
    init();
}

void SpinBoxBase::init()
{
    setFocusPolicy(Qt::StrongFocus);
    StyleSheetBase::apply(this, StyleSheetBase::SpinBox);
}

void SpinBoxBase::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

void SpinBoxBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
}

void SpinBoxBase::setTransparent(bool transparent)
{
    setProperty("isTransparent", transparent);
}

void SpinBoxBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

// DoubleSpinBoxBase implementation
DoubleSpinBoxBase::DoubleSpinBoxBase(QWidget *parent)
    : QDoubleSpinBox(parent)
{
    init();
}

void DoubleSpinBoxBase::init()
{
    setFocusPolicy(Qt::StrongFocus);
    StyleSheetBase::apply(this, StyleSheetBase::SpinBox);
}

void DoubleSpinBoxBase::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

void DoubleSpinBoxBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
}

void DoubleSpinBoxBase::setTransparent(bool transparent)
{
    setProperty("isTransparent", transparent);
}

void DoubleSpinBoxBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

} // namespace QEW
