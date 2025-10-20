#include "ComboBox.h"

#include "../Common/StyleSheet.h"


namespace QEW {

ComboBoxBase::ComboBoxBase(QWidget *parent)
    : QComboBox(parent)
{
    init();
}

void ComboBoxBase::init()
{
    setFocusPolicy(Qt::StrongFocus);
    StyleSheetBase::apply(this, StyleSheetBase::ComboBox);
}

void ComboBoxBase::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

void ComboBoxBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
}

void ComboBoxBase::setTransparent(bool transparent)
{
    setProperty("isTransparent", transparent);
}

void ComboBoxBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

} // namespace QEW
