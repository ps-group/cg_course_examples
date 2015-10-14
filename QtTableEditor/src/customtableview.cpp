#include "customtableview.h"

CustomTableView::CustomTableView(QWidget *parent)
    : QTableView(parent)
{
}

std::set<int> CustomTableView::selectedRows() const
{
    std::set<int> rows;
    for (const auto &index : selectedIndexes())
    {
        rows.insert(index.row());
    }
    return rows;
}

