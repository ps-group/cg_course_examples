#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QTime>
#include <QTimer>
#include "cube2d.h"

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *);

private:
    Cube2D m_cube2d;
    QTime m_time;
    QTimer m_timer;
};

#endif // MAINWINDOW_H
