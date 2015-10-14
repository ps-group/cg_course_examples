#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statstablemodel.h"
#include "statsdocument.h"
#include "insertrowdialog.h"
#include "aboutdialog.h"
#include <QTableView>
#include <QDebug>
#include <unordered_set>
#include <QMessageBox>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_tableModel = std::make_shared<StatsTableModel>();
    m_ui->tableData->setModel(m_tableModel.get());
    m_ui->tableData->setAlternatingRowColors(true);
    m_ui->tableData->setSortingEnabled(true);

    m_ui->newDocument->setShortcut(QKeySequence(QKeySequence::New));
    m_ui->openDocument->setShortcut(QKeySequence(QKeySequence::Open));
    m_ui->saveDocument->setShortcut(QKeySequence(QKeySequence::Save));
    m_ui->saveDocumentAs->setShortcut(QKeySequence(QKeySequence::SaveAs));

    m_document.reset(new StatsDocument(this, *m_tableModel));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::on_newDocument_triggered()
{
    m_document->createNew();
}

void MainWindow::on_saveDocument_triggered()
{
    m_document->save();
}

void MainWindow::on_openDocument_triggered()
{
    m_document->open();
}

void MainWindow::on_saveDocumentAs_triggered()
{
    m_document->saveAs();
}

void MainWindow::on_showTable_triggered()
{
}

void MainWindow::on_showDiagram_triggered()
{
}

void MainWindow::on_actionAbout_triggered()
{
    std::unique_ptr<AboutDialog> dialog(new AboutDialog(this));
    dialog->exec();
}

void MainWindow::on_actionInsertRow_triggered()
{
    std::unique_ptr<InsertRowDialog> dialog(new InsertRowDialog(this));
    connect(dialog.get(), SIGNAL(rowReady(QString, int)), this, SLOT(onRowReady(QString, int)));
    dialog->exec();
}

void MainWindow::onRowReady(QString text, int value)
{
    auto model = m_tableModel->statsModel();
    model.append(text, value);
    m_tableModel->setStatsModel(model);
}

void MainWindow::on_actionDeleteRow_triggered()
{
    std::set<int> deletedRows = m_ui->tableData->selectedRows();
    if (deletedRows.size() == 0)
    {
        return;
    }

    auto statsModel = m_tableModel->statsModel();
    StatsKeyValueModel newModel;
    for (size_t i = 0, n = statsModel.size(); i < n; ++i)
    {
        if (deletedRows.count(i))
        {
            continue;
        }
        newModel.append(statsModel.key(i), statsModel.value(i));
    }
    m_tableModel->setStatsModel(newModel);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QRect tableArea = QRect(QPoint(0, 0), event->size());
    m_ui->tableData->setGeometry(tableArea);
}
