#include "Menu.h"

#include "../Common/StyleSheet.h"


namespace QEW {

MenuBase::MenuBase(QWidget *parent)
    : QMenu(parent)
{
    init();
}

void MenuBase::init()
{
    StyleSheetBase::apply(this, StyleSheetBase::Menu);
}

void MenuBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

} // namespace QEW
