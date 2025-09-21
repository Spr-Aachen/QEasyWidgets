#include "List.h"

#include <QFont>
#include <QAction>
#include <QMenu>
#include <QMap>

#include "../Common/StyleSheet.h"
#include "../Common/QFunctions.h"


/**
 * ListBase implementation
 */

ListBase::ListBase(QWidget *parent)
    : QListView(parent)
    , m_model(nullptr) {
    init();
}

void ListBase::init() {
    m_model = new QStandardItemModel(this);
    setModel(m_model);

    connect(this, &QListView::clicked, this, &ListBase::onItemClicked);

    QFont font = this->font();
    font.setPointSize(12);
    setFont(font);

    StyleSheetBase::apply(this, StyleSheetBase::List);
}

QStandardItemModel *ListBase::model() const {
    return m_model;
}

int ListBase::row(const QStandardItem *item) const {
    if (!item) return -1;
    QModelIndex index = m_model->indexFromItem(item);
    return index.row();
}

void ListBase::insertRow(int row, QStandardItem *item) {
    if (item) {
        m_model->insertRow(row, item);
    }
}

void ListBase::removeRow(int row) {
    m_model->removeRow(row);
}

int ListBase::rowCount() const {
    return m_model->rowCount();
}

void ListBase::setRowCount(int rows) {
    m_model->setRowCount(rows);
}

QStandardItem *ListBase::item(int row) const {
    return m_model->item(row);
}

void ListBase::setItem(int row, QStandardItem *item) {
    if (item) {
        m_model->setItem(row, item);
    }
}

void ListBase::addItem(QStandardItem *item) {
    if (!item) return;
    m_model->appendRow(item);
}

QStandardItem *ListBase::takeItem(int row) {
    QList<QStandardItem*> list = m_model->takeRow(row);
    if (list.isEmpty()) return nullptr;
    return list.first();
}

QStandardItem *ListBase::currentItem() const {
    QModelIndexList indexes = selectedIndexes();
    if (!indexes.isEmpty()) {
        return m_model->itemFromIndex(indexes.first());
    }
    return nullptr;
}

void ListBase::setCurrentItem(QStandardItem *item) {
    if (!item) return;
    QModelIndex index = m_model->indexFromItem(item);
    setCurrentIndex(index);
}

void ListBase::clear() {
    m_model->clear();
}

void ListBase::click(QStandardItem *item) {
    if (!item) return;
    QModelIndex index = m_model->indexFromItem(item);
    setCurrentIndex(index);
    emit clicked(index);
}

MenuBase *ListBase::contextMenu() const {
    if (!m_contextMenu) {
        // const_cast because method is const; m_contextMenu is lazily created
        const_cast<ListBase*>(this)->m_contextMenu = new MenuBase(const_cast<ListBase*>(this));
    }
    return m_contextMenu;
}

void ListBase::setContextMenu(MenuBase *menu) {
    m_contextMenu = menu;
}

void ListBase::setContextMenu(const QMap<QString, std::function<void()>> &actions) {
    // Ensure we have a menu
    if (!m_contextMenu) {
        m_contextMenu = new MenuBase(this);
    }

    // Make sure context menu policy is set and connect once
    if (contextMenuPolicy() != Qt::CustomContextMenu) {
        setContextMenuPolicy(Qt::CustomContextMenu);
    }

    // Use UniqueConnection to avoid duplicate connections
    QObject::connect(this, &QWidget::customContextMenuRequested, this, [this, actions](const QPoint &position) {
        if (!m_contextMenu) m_contextMenu = new MenuBase(this);
        m_contextMenu->clear();
        showContextMenu(this, m_contextMenu, actions, mapToGlobal(position));
    }, Qt::UniqueConnection);
}

void ListBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
}

void ListBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void ListBase::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    QListView::currentChanged(current, previous);
    QStandardItem *currentItem = m_model->itemFromIndex(current);
    QStandardItem *previousItem = m_model->itemFromIndex(previous);
    emit currentItemChanged(currentItem, previousItem);
}

void ListBase::onItemClicked(const QModelIndex &index) {
    if (index.isValid()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        emit itemClicked(item);
    }
}