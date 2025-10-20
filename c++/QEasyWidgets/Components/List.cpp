#include "List.h"

#include <QFont>


namespace QEW {

ListBase::ListBase(QWidget *parent)
    : QListView(parent)
    , m_model(nullptr)
{
    init();
}

void ListBase::init()
{
    m_model = new QStandardItemModel(this);
    setModel(m_model);

    connect(this, &QListView::clicked, this, &ListBase::onItemClicked);

    QFont font = this->font();
    font.setPointSize(12);
    setFont(font);

    // TODO: Apply stylesheet
    // StyleSheetBase::apply(this, StyleSheetBase::List);
}

QStandardItemModel *ListBase::model() const
{
    return m_model;
}

int ListBase::row(const QStandardItem *item) const
{
    if (!item) return -1;
    QModelIndex index = m_model->indexFromItem(item);
    return index.row();
}

void ListBase::insertRow(int row, QStandardItem *item)
{
    if (item) {
        m_model->insertRow(row, item);
    }
}

void ListBase::removeRow(int row)
{
    m_model->removeRow(row);
}

int ListBase::rowCount() const
{
    return m_model->rowCount();
}

void ListBase::setRowCount(int rows)
{
    m_model->setRowCount(rows);
}

QStandardItem *ListBase::item(int row) const
{
    return m_model->item(row);
}

void ListBase::setItem(int row, QStandardItem *item)
{
    if (item) {
        m_model->setItem(row, item);
    }
}

void ListBase::onItemClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        emit itemClicked(item);
    }
}

} // namespace QEW
