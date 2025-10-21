#include "Tree.h"

#include <QHeaderView>
#include <QBrush>
#include <QVariant>

#include "../Common/Theme.h"
#include "../Common/StyleSheet.h"


/**
 * TreeItemDelegate implementation
 */

TreeItemDelegate::TreeItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
}

void TreeItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const {
    QStyledItemDelegate::initStyleOption(option, index);

    // Set text color based on theme
    QColor textColor = isDarkTheme() ? QColor(Qt::white) : QColor(Qt::black);
    QVariant v = index.data(Qt::ForegroundRole);
    if (v.canConvert<QBrush>()) {
        const QBrush brush = qvariant_cast<QBrush>(v);
        if (brush.color().isValid()) {
            textColor = brush.color();
        }
    }
    option->palette.setColor(QPalette::Text, textColor);
    option->palette.setColor(QPalette::HighlightedText, textColor);
}

/**
 * TreeWidgetBase implementation
 */

TreeWidgetBase::TreeWidgetBase(QWidget *parent)
    : QTreeWidget(parent) {
    init();
}

void TreeWidgetBase::init() {
    setColumnCount(1);
    header()->setHighlightSections(false);
    header()->setDefaultAlignment(Qt::AlignCenter);
    setItemDelegate(new TreeItemDelegate(this));
    setIconSize(QSize(16, 16));

    StyleSheetBase::apply(this, StyleSheetBase::Tree);
}

QList<QTreeWidgetItem*> TreeWidgetBase::rootItems() const {
    QList<QTreeWidgetItem*> items;
    items.reserve(topLevelItemCount());
    for (int i = 0; i < topLevelItemCount(); ++i) {
        items.append(topLevelItem(i));
    }
    return items;
}

QStringList TreeWidgetBase::rootItemTexts() const {
    QStringList texts;
    texts.reserve(topLevelItemCount());
    for (int i = 0; i < topLevelItemCount(); ++i) {
        if (auto *it = topLevelItem(i))
            texts.append(it->text(0));
    }
    return texts;
}

QList<QTreeWidgetItem*> TreeWidgetBase::childItems(QTreeWidgetItem *root) const {
    QList<QTreeWidgetItem*> items;
    if (!root)
        return items;
    items.reserve(root->childCount());
    for (int i = 0; i < root->childCount(); ++i) {
        items.append(root->child(i));
    }
    return items;
}

QStringList TreeWidgetBase::childItemTexts(QTreeWidgetItem *root) const {
    QStringList texts;
    if (!root)
        return texts;
    texts.reserve(root->childCount());
    for (int i = 0; i < root->childCount(); ++i) {
        if (auto *it = root->child(i))
            texts.append(it->text(0));
    }
    return texts;
}

void TreeWidgetBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}