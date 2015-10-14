#pragma once

#include <QMainWindow>
#include <QString>
#include <memory>

namespace Ui {
class MainWindow;
}
class StatsTableModel;
class StatsDocument;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_newDocument_triggered();
    void on_saveDocument_triggered();
    void on_openDocument_triggered();
    void on_saveDocumentAs_triggered();
    void on_showTable_triggered();
    void on_showDiagram_triggered();

    void on_actionAbout_triggered();

    void on_actionInsertRow_triggered();
    void onRowReady(QString text, int value);

    void on_actionDeleteRow_triggered();

private:
    Ui::MainWindow *m_ui;
    std::shared_ptr<StatsTableModel> m_tableModel;
    std::unique_ptr<StatsDocument> m_document;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *);
};
