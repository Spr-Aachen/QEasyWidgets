#ifndef QEASYWIDGETS_TREE_H
#define QEASYWIDGETS_TREE_H

#include <QTreeWidget>
#include <QStyledItemDelegate>


namespace QEW {

/**
 * @brief Custom item delegate for tree widget
 */
class TreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TreeItemDelegate(QObject *parent = nullptr);
    ~TreeItemDelegate() override = default;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;
};

/**
 * @brief Enhanced tree widget with theme support
 */
class TreeWidgetBase : public QTreeWidget
{
    Q_OBJECT

public:
    explicit TreeWidgetBase(QWidget *parent = nullptr);
    ~TreeWidgetBase() override = default;

private:
    void init();
};

} // namespace QEW

#endif // QEASYWIDGETS_TREE_H
