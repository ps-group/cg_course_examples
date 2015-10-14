#include "insertrowdialog.h"
#include "ui_insertrowdialog.h"

InsertRowDialog::InsertRowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertRowDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
}

InsertRowDialog::~InsertRowDialog()
{
    delete ui;
}

void InsertRowDialog::onAccepted()
{
    emit rowReady(ui->editName->text(), ui->editValue->value());
}
