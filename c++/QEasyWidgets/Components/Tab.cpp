#include "Tab.h"

#include <QFont>

#include "../Common/StyleSheet.h"


namespace QEW {

TabWidgetBase::TabWidgetBase(QWidget *parent)
    : SizableWidget<QTabWidget>(parent)
{
    init();
}

void TabWidgetBase::init()
{
    tabBar()->setMinimumSize(84, 42);

    QFont font = this->font();
    font.setPointSize(21);
    setFont(font);

    StyleSheetBase::apply(this, StyleSheetBase::Tab);
}

void TabWidgetBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
}

void TabWidgetBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

} // namespace QEW
