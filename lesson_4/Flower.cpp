#include "stdafx.h"
#include "Flower.h"
#include <cmath>

CFlower::~CFlower()
{
    DeleteList();
}

void CFlower::SetupShape(float minRadius, float maxRadius, int petalCount)
{
    DeleteList();
    m_minRadius = minRadius;
    m_maxRadius = maxRadius;
    m_petalCount = petalCount;
}

void CFlower::SetCenterColor(const glm::vec3 &centerColor)
{
    m_centerColor = centerColor;
}

void CFlower::SetPetalColor(const glm::vec3 &petalColor)
{
    m_petalColor = petalColor;
}

void CFlower::Draw() const
{
    // если дисплейный список еще не был создан, то для начала создаем его
    if (!m_displayList)
    {
        // Генерируем новый дисплейный список для цветка
        m_displayList = glGenLists(1);
        glNewList(m_displayList, GL_COMPILE);
        // Для заполнения списка вызываем redraw.
        Redraw();
        // завершаем дисплейный список
        glEndList();
    }

    // вызываем ранее созданный дисплейный список
    glCallList(m_displayList);
}

bool CFlower::HitTest(const glm::vec2 &point) const
{
    // центр всегда в точке (0, 0), так что вектор от центра
    // к point равен point; извлекаем полярный угол этого вектора.
    const float angle = std::atan2(point.y, point.x);
    const float radius = GetRadiusByAngle(angle);

    // Если длина вектора меньше радиуса, есть попадание.
    // Мы используем `glm::length` вместо point.length(), потому что
    // point.length() вернёт число компонентов вектора, т. е. число 2.
    return (glm::length(point) < radius);
}

void CFlower::DeleteList()
{
    if (m_displayList)
    {
        glDeleteLists(m_displayList, 1);
        m_displayList = 0;
    }
}

void CFlower::Redraw() const
{
    // угловой шаг (для рисования каждого лепестка
    // нам понадобится как минимум 20 вершин)
    const float step = float(2 * M_PI) / float(m_petalCount * 20);

    glBegin(GL_TRIANGLE_FAN);

    // центральная вершина будет иметь цвет m_centerColor
    glColor3f(m_centerColor.x, m_centerColor.y, m_centerColor.z);
    glVertex2f(0, 0);

    // вершины лепестка будут иметь цвет m_petalColor
    glColor3f(m_petalColor.x, m_petalColor.y, m_petalColor.z);
    for (float angle = 0; angle <= float(2 * M_PI) + 0.5f * step; angle += step)
    {
        // в дискретном мире компьютеров sin(2 * PI) может отличаться
        // от sin(0). Поэтому если angle подошел близко к 2*PI,
        // считаем его равным 0
        // это нужно для того, чтобы начало и конец веера
        // сошлись в одной точке
        float fixedAngle = (fabsf(angle - float(2 * M_PI)) < 1e-4f) ? 0 : angle;

        // вычисляем радиус искривленной окружности для данного угла
        float radius = GetRadiusByAngle(fixedAngle);
        float x = radius * cosf(fixedAngle);
        float y = radius * sinf(fixedAngle);

        // создаем новую вершину
        glVertex2f(x, y);
    }

    glEnd();
}

float CFlower::GetRadiusByAngle(float angle)const
{
    // вычисляем радиус искривленной окружности для данного угла.
    return 0.5f * (m_maxRadius - m_minRadius)
            * cosf(angle * m_petalCount) + m_minRadius;
}
