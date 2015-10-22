#include "maindialog.h"
#include <QPainter>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    m_cube2d(200.f)
{
    m_time.start();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.setInterval(20);
    m_timer.setSingleShot(false);
    m_timer.start(m_timer.interval());
}

MainDialog::~MainDialog()
{
}


void MainDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    m_cube2d.advance(0.001f * m_time.elapsed());
    m_time.restart();

    int PADDING = 10;
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    m_cube2d.render(painter, QRect(PADDING, PADDING,
                                   width() - 2 * PADDING,
                                   height() - 2 * PADDING));
}
