#include "stdafx.h"
#include "MovableFlower.h"

void CMovableFlower::Draw() const
{
    // Модифицируем Model-View матрицу,
    // теперь она задаёт перемещение на вектор (x, y, 0)
    glm::vec3 offset = { m_position.x, m_position.y, 0.f };
    glm::mat4 transform = glm::translate(glm::mat4(), offset);
    // Сохраняем старую матрицу в стек матриц драйвера
    glPushMatrix();
    glLoadMatrixf(glm::value_ptr(transform));
    // Способ рисования не изменился
    CFlower::Draw();
    // Извлекаем старую матрицу из стека матриц драйвера
    glPopMatrix();
}

bool CMovableFlower::HitTest(const glm::vec2 &point) const
{
    // вызываем CFlower::HitTest со смещённой проверяемой точкой.
    return CFlower::HitTest(point - m_position);
}

const glm::vec2 &CMovableFlower::GetPosition() const
{
    return m_position;
}

void CMovableFlower::SetPosition(const glm::vec2 &position)
{
    m_position = position;
}
