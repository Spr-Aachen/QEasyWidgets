#ifndef QEASYWIDGETS_LIST_H
#define QEASYWIDGETS_LIST_H

#include <QListView>
#include <QStandardItemModel>


namespace QEW {

/**
 * @brief Enhanced list view with theme support
 */
class ListBase : public QListView
{
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

signals:
    void currentItemChanged(QStandardItem *current, QStandardItem *previous);
    void itemClicked(QStandardItem *item);

private slots:
    void onItemClicked(const QModelIndex &index);

private:
    void init();

    QStandardItemModel *m_model;
};

} // namespace QEW

#endif // QEASYWIDGETS_LIST_H
