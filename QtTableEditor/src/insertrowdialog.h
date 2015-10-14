#ifndef INSERTROWDIALOG_H
#define INSERTROWDIALOG_H

#include <QDialog>

namespace Ui {
class InsertRowDialog;
}

class InsertRowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertRowDialog(QWidget *parent = 0);
    ~InsertRowDialog();

signals:
    void rowReady(QString text, int value);

private slots:
    void onAccepted();

private:
    Ui::InsertRowDialog *ui;
};

#endif // INSERTROWDIALOG_H
