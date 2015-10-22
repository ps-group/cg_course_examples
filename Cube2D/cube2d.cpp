#define _USE_MATH_DEFINES
#include "cube2d.h"
#include <math.h>
#include <QVector3D>
#include <QVector2D>

static const int WIRED_LINE_WIDTH = 5;

enum {LEFT,  RIGHT,  ON_LINE };

int classify(const QPointF &center, const QPointF &lineStart, const QPointF &lineEnd)
{
  QPointF a = lineEnd - lineStart;
  QPointF b = center - lineStart;
  double sa = a.x() * b.y() - b.x() * a.y();
  if (sa < 0.0)
    return LEFT;
  if (sa > 0.0)
    return RIGHT;
  return ON_LINE;
}

Cube2D::Cube2D(float size)
    : m_size(size)
{
}

void Cube2D::advance(float deltaTime)
{
    m_angle += m_rotateSpeed * deltaTime;
    (void)deltaTime;
}

void Cube2D::render(QPainter &painter, const QRectF &viewport)
{
    QVector3D verts3d[8] = {
        // top quad
        QVector3D(+0.5f * m_size, +0.5f * m_size, +0.5f * m_size),
        QVector3D(+0.5f * m_size, -0.5f * m_size, +0.5f * m_size),
        QVector3D(-0.5f * m_size, -0.5f * m_size, +0.5f * m_size),
        QVector3D(-0.5f * m_size, +0.5f * m_size, +0.5f * m_size),
        // bottom quad
        QVector3D(+0.5f * m_size, +0.5f * m_size, -0.5f * m_size),
        QVector3D(+0.5f * m_size, -0.5f * m_size, -0.5f * m_size),
        QVector3D(-0.5f * m_size, -0.5f * m_size, -0.5f * m_size),
        QVector3D(-0.5f * m_size, +0.5f * m_size, -0.5f * m_size)
    };

    std::vector<QVector2D> points(8);
    for (size_t i = 0; i < points.size(); ++i)
    {
        points[i] = project3D(rotateAroundZ(verts3d[i], QVector2D(0, 0), m_angle));
    }

    QColor cubeColor(Qt::green);
    painter.setPen(QPen(Qt::NoPen));

    painter.setBrush(cubeColor);
    drawQuad(viewport.center(), points, {0, 1, 2, 3}, painter);
    drawQuad(viewport.center(), points, {7, 6, 5, 4}, painter);

    painter.setBrush(cubeColor.darker());
    drawQuad(viewport.center(), points, {0, 4, 5, 1}, painter);
    drawQuad(viewport.center(), points, {1, 5, 6, 2}, painter);
    drawQuad(viewport.center(), points, {2, 6, 7, 3}, painter);
    drawQuad(viewport.center(), points, {3, 7, 4, 0}, painter);
}

float Cube2D::size() const
{
    return m_size;
}

void Cube2D::setSize(float size)
{
    m_size = size;
}
float Cube2D::angle() const
{
    return m_angle;
}

void Cube2D::setAngle(float angle)
{
    m_angle = angle;
}
float Cube2D::rotateSpeed() const
{
    return m_rotateSpeed;
}

void Cube2D::setRotateSpeed(float rotateSpeed)
{
    m_rotateSpeed = rotateSpeed;
}

QVector2D Cube2D::project3D(const QVector3D &point)
{
    const float SIN_30 = 0.5f;
    const float SIN_60 = 0.5f * sqrt(3.f);
    QVector2D result;
    result.setX(SIN_60 * (point.x() + point.y()));
    result.setY(-point.z() + SIN_30 * (point.y() - point.x()));

    return result;
}

float Cube2D::degreesToRadians(float deg)
{
    return deg * M_PI / 180.0;
}

QVector3D Cube2D::rotateAroundZ(const QVector3D &point, const QVector2D &anchorPoint, float deltaAngle)
{
    QVector2D delta = point.toVector2D() - anchorPoint;
    float angleRadians = asin(delta.y() / delta.length());
    if (delta.x() < 0)
    {
        angleRadians = M_PI - angleRadians;
    }
    angleRadians += degreesToRadians(deltaAngle);

    QVector2D ret = anchorPoint + QVector2D(sin(angleRadians) * delta.length(), cos(angleRadians) * delta.length());
    return QVector3D(ret.x(), ret.y(), point.z());
}

// Takes 4 verticies from array with 8 verticies and draws quad.
void Cube2D::drawQuad(QPointF center, std::vector<QVector2D> verticies, std::vector<int> indicies, QPainter &painter)
{
    std::array<QPointF, 4> quad = {
        QPointF(center + verticies[indicies[0]].toPointF()),
        QPointF(center + verticies[indicies[1]].toPointF()),
        QPointF(center + verticies[indicies[2]].toPointF()),
        QPointF(center + verticies[indicies[3]].toPointF())
    };
    // Center always is on right side, render quad
    if (!isBackCulled(quad))
    {
        painter.drawPolygon(quad.data(), 4);
    }
}

// Returns false if central point is on right side for each edge.
bool Cube2D::isBackCulled(const std::array<QPointF, 4> &quad)
{
    QPointF center(0, 0);
    for (const QPointF &point : quad)
    {
        center += point;
    }
    center /= float(quad.size());

    QPointF prevPoint = quad[3];
    for (const QPointF &point : quad)
    {
        if (RIGHT == classify(center, point, prevPoint))
        {
            return true;
        }
        prevPoint = point;
    }
    return false;
}



