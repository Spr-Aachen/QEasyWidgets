#include "Tree.h"

#include "../Common/StyleSheet.h"
#include "../Common/Theme.h"


namespace QEW {

TreeItemDelegate::TreeItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TreeItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    // Set text color based on theme
    QColor textColor = isDarkTheme() ? QColor(Qt::white) : QColor(Qt::black);
    option->palette.setColor(QPalette::Text, textColor);
    option->palette.setColor(QPalette::HighlightedText, textColor);
}

TreeWidgetBase::TreeWidgetBase(QWidget *parent)
    : QTreeWidget(parent)
{
    init();
}

void TreeWidgetBase::init()
{
    setColumnCount(1);
    header()->setHighlightSections(false);
    header()->setDefaultAlignment(Qt::AlignCenter);
    setItemDelegate(new TreeItemDelegate(this));
    setIconSize(QSize(16, 16));

    StyleSheetBase::apply(this, StyleSheetBase::Tree);
}

} // namespace QEW
