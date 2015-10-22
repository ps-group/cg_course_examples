#pragma once

#include <QPainter>
#include <vector>

class Cube2D
{
public:
    Cube2D(float size);

    void advance(float deltaTime);
    void render(QPainter &painter, const QRectF &viewport);

    float size() const;
    void setSize(float size);

    float angle() const;
    void setAngle(float angle);

    float rotateSpeed() const;
    void setRotateSpeed(float rotateSpeed);

private:
    static QVector2D project3D(const QVector3D &point);
    static float degreesToRadians(float deg);
    static QVector3D rotateAroundZ(const QVector3D &point, const QVector2D &anchorPoint, float deltaAngle);
    static void drawQuad(QPointF center, std::vector<QVector2D> verticies, std::vector<int> indicies, QPainter &painter);
    static bool isBackCulled(const std::array<QPointF, 4> &quad);

    float m_size = 0;
    float m_angle = 0; // in degreees
    float m_rotateSpeed = 90.f; // in degreees
};
