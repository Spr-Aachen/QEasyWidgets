#include "ToolBox.h"

#include "../Common/StyleSheet.h"


namespace QEW {

ToolBoxBase::ToolBoxBase(QWidget *parent)
    : QToolBox(parent)
{
    init();
}

void ToolBoxBase::init()
{
    StyleSheetBase::apply(this, StyleSheetBase::ToolBox);
}

void ToolBoxBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

} // namespace QEW
