#ifndef QEASYWIDGETS_TREE_H
#define QEASYWIDGETS_TREE_H

#include <QTreeWidget>
#include <QStyledItemDelegate>
#include <QList>
#include <QStringList>

/**
 * Custom item delegate for tree widget
 */
class TreeItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit TreeItemDelegate(QObject *parent = nullptr);
    ~TreeItemDelegate() override = default;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;
};


/**
 * Enhanced tree widget with theme support
 */
class TreeWidgetBase : public QTreeWidget {
    Q_OBJECT

public:
    explicit TreeWidgetBase(QWidget *parent = nullptr);
    ~TreeWidgetBase() override = default;

    QList<QTreeWidgetItem*> rootItems() const;
    QStringList rootItemTexts() const;
    QList<QTreeWidgetItem*> childItems(QTreeWidgetItem *root) const;
    QStringList childItemTexts(QTreeWidgetItem *root) const;

    void clearDefaultStyleSheet();

private:
    void init();
};


#endif // QEASYWIDGETS_TREE_H