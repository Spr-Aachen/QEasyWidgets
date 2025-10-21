#include "GroupBox.h"

#include <QFont>
#include <QPropertyAnimation>

#include "../Common/StyleSheet.h"
#include "../Common/QFunctions.h"


namespace QEW {

GroupBoxBase::GroupBoxBase(QWidget *parent)
    : SizableWidget<QGroupBox>(parent)
{
    init();
}

GroupBoxBase::GroupBoxBase(const QString &title, QWidget *parent)
    : SizableWidget<QGroupBox>(title, parent)
{
    init();
}

void GroupBoxBase::init()
{
    setCheckable(true);
    connect(this, &QGroupBox::toggled, this, [this](bool checked) {
        if (checked) {
            expand();
        } else {
            collapse();
        }
    });

    QFont font = this->font();
    font.setPointSize(15);
    setFont(font);

    StyleSheetBase::apply(this, StyleSheetBase::GroupBox);
}

void GroupBoxBase::setBorderless(bool borderless)
{
    setProperty("isBorderless", borderless);
}

void GroupBoxBase::setTransparent(bool transparent)
{
    setProperty("isTransparent", transparent);
}

void GroupBoxBase::clearDefaultStyleSheet()
{
    // TODO: Implement deregistration if StyleSheetBase supports it
}

void GroupBoxBase::expand()
{
    // TODO: Implement expand animation
    // For now, just ensure all children are visible
    QList<QWidget*> children = findChildren<QWidget*>();
    for (QWidget *child : children) {
        if (child->parent() == this) {
            child->setVisible(true);
        }
    }
}

void GroupBoxBase::collapse()
{
    // TODO: Implement collapse animation
    // For now, hide all children except title
    QList<QWidget*> children = findChildren<QWidget*>();
    for (QWidget *child : children) {
        if (child->parent() == this) {
            child->setVisible(false);
        }
    }
}

} // namespace QEW
