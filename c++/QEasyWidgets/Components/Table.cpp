#include "Table.h"

#include <QHeaderView>

#include "../Common/StyleSheet.h"
#include "../Common/Theme.h"


/**
 * TableItemDelegate implementation
 */

TableItemDelegate::TableItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_margin(3) {
}

QSize TableItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(33);
    size = size.grownBy(QMargins(m_margin, 2 * m_margin, m_margin, 2 * m_margin));
    return size;
}

void TableItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const {
    QStyledItemDelegate::initStyleOption(option, index);

    // Set text color based on theme
    QColor textColor = isDarkTheme() ? QColor(Qt::white) : QColor(Qt::black);
    option->palette.setColor(QPalette::Text, textColor);
    option->palette.setColor(QPalette::HighlightedText, textColor);
}

/**
 * TableBase implementation
 */

TableBase::TableBase(QWidget *parent)
    : QTableView(parent)
    , m_model(nullptr)
    , m_isIndexShown(false) {
    init();
}

void TableBase::init() {
    m_model = new QStandardItemModel(this);
    setModel(m_model);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    verticalHeader()->setStretchLastSection(false);
    verticalHeader()->setResizeContentsPrecision(0);
    verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(false);
    horizontalHeader()->setResizeContentsPrecision(0);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    setSelectionMode(QAbstractItemView::NoSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(true);

    // Add index column
    m_model->insertColumn(0);
    m_model->setHorizontalHeaderItem(0, new QStandardItem("Index"));
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    connect(m_model, &QStandardItemModel::rowsInserted, this, &TableBase::setIndex);
    connect(m_model, &QStandardItemModel::rowsRemoved, this, &TableBase::setIndex);

    setItemDelegate(new TableItemDelegate(this));

    StyleSheetBase::apply(this, StyleSheetBase::Table);

    setIndexHeaderVisible(true);
}

QStandardItemModel *TableBase::model() const {
    return m_model;
}

int TableBase::currentRow() const {
    return QTableView::currentIndex().row();
}

int TableBase::currentColumn() const {
    return QTableView::currentIndex().column() - 1; // Adjust for index column
}

void TableBase::insertRow(int row) {
    m_model->insertRow(row);
}

void TableBase::removeRow(int row) {
    m_model->removeRow(row);
}

int TableBase::rowCount() const {
    return m_model->rowCount();
}

void TableBase::setRowCount(int rows) {
    m_model->setRowCount(rows);
}

void TableBase::insertColumn(int column) {
    m_model->insertColumn(column + 1); // Adjust for index column
}

void TableBase::removeColumn(int column) {
    m_model->removeColumn(column + 1); // Adjust for index column
}

int TableBase::columnCount() const {
    return m_model->columnCount() - 1; // Exclude index column
}

void TableBase::setColumnCount(int columns) {
    m_model->setColumnCount(columns + 1); // Include index column
}

QStandardItem *TableBase::item(int row, int column) const {
    return m_model->item(row, column + 1); // Adjust for index column
}

void TableBase::setItem(int row, int column, QStandardItem *item) {
    m_model->setItem(row, column + 1, item); // Adjust for index column
}

QWidget *TableBase::cellWidget(int row, int column) const {
    return QTableView::indexWidget(m_model->index(row, column + 1));
}

void TableBase::setCellWidget(int row, int column, QWidget *widget) {
    QTableView::setIndexWidget(m_model->index(row, column + 1), widget);
}

void TableBase::setHorizontalHeaderItem(int column, QStandardItem *item) {
    m_model->setHorizontalHeaderItem(column + 1, item); // Adjust for index column
}

void TableBase::setHorizontalHeaderLabels(const QStringList &labels) {
    for (int i = 0; i < labels.size(); ++i) {
        if (i >= columnCount()) break;
        setHorizontalHeaderItem(i, new QStandardItem(labels.at(i)));
    }
}

void TableBase::setBorderless(bool borderless) {
    setProperty("isBorderless", borderless);
}

void TableBase::clearDefaultStyleSheet() {
    StyleSheetBase::deregistrate(this);
}

void TableBase::setIndex() {
    for (int i = 0; i < m_model->rowCount(); ++i) {
        m_model->setItem(i, 0, new QStandardItem(QString::number(i + 1)));
    }
}

void TableBase::setIndexHeaderVisible(bool showIndexHeader) {
    if (showIndexHeader && !m_isIndexShown) {
        showColumn(0);
        m_isIndexShown = true;
    } else if (!showIndexHeader && m_isIndexShown) {
        hideColumn(0);
        m_isIndexShown = false;
    }
}