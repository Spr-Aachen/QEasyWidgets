#ifndef QEASYWIDGETS_TABLE_H
#define QEASYWIDGETS_TABLE_H

#include <QTableView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>


namespace QEW {

/**
 * @brief Custom item delegate for table
 */
class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ItemDelegate(QObject *parent = nullptr);
    ~ItemDelegate() override = default;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;

private:
    int m_margin;
};

/**
 * @brief Enhanced table view with theme support
 */
class TableBase : public QTableView
{
    Q_OBJECT

public:
    explicit TableBase(QWidget *parent = nullptr);
    ~TableBase() override = default;

    QStandardItemModel *model() const;

    int currentRow() const;
    int currentColumn() const;

    void insertRow(int row);
    void removeRow(int row);
    int rowCount() const;
    void setRowCount(int rows);

    void insertColumn(int column);
    void removeColumn(int column);
    int columnCount() const;
    void setColumnCount(int columns);

    QStandardItem *item(int row, int column) const;
    void setItem(int row, int column, QStandardItem *item);

    QWidget *cellWidget(int row, int column) const;
    void setCellWidget(int row, int column, QWidget *widget);

    void setHorizontalHeaderItem(int column, QStandardItem *item);
    void setHorizontalHeaderLabels(const QStringList &labels);

    void setBorderless(bool borderless);

    void clearDefaultStyleSheet();

signals:
    void sorted();

private slots:
    void setIndex();

private:
    void init();

    QStandardItemModel *m_model;
    bool m_isIndexShown;
};

} // namespace QEW

#endif // QEASYWIDGETS_TABLE_H
