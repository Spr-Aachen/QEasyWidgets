#ifndef QEASYWIDGETS_LIST_H
#define QEASYWIDGETS_LIST_H

#include <QListView>
#include <QStandardItemModel>
#include <QMenu>
#include <QMap>
#include <functional>

#include "Menu.h"


/**
 * Enhanced list view with theme support
 */
class ListBase : public QListView {
    Q_OBJECT

public:
    explicit ListBase(QWidget *parent = nullptr);
    ~ListBase() override = default;

    QStandardItemModel *model() const;

    int row(const QStandardItem *item) const;

    void insertRow(int row, QStandardItem *item);
    void removeRow(int row);
    int rowCount() const;
    void setRowCount(int rows);

    QStandardItem *item(int row) const;
    void setItem(int row, QStandardItem *item);

    void addItem(QStandardItem *item);
    QStandardItem *takeItem(int row);

    QStandardItem *currentItem() const;
    void setCurrentItem(QStandardItem *item);

    void clear();
    void click(QStandardItem *item);

    // Context menu helpers
    MenuBase *contextMenu() const;
    void setContextMenu(MenuBase *menu);
    void setContextMenu(const QMap<QString, std::function<void()>> &actions);

    void setBorderless(bool borderless);
    void clearDefaultStyleSheet();

signals:
    void currentItemChanged(QStandardItem *current, QStandardItem *previous);
    void itemClicked(QStandardItem *item);

private slots:
    void onItemClicked(const QModelIndex &index);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private:
    void init();

    QStandardItemModel *m_model;
    MenuBase *m_contextMenu = nullptr;
};


#endif // QEASYWIDGETS_LIST_H