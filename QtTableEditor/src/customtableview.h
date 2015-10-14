#pragma once

#include <QTableView>
#include <set>

class CustomTableView : public QTableView
{
    Q_OBJECT
public:
    CustomTableView(QWidget *parent = 0);

    std::set<int> selectedRows() const;
};
