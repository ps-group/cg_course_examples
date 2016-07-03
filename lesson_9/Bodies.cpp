#include "stdafx.h"
#include "Bodies.h"
#include <stdint.h>
#include <type_traits>

namespace
{

template <typename T, size_t N>
char ( &_ArraySizeHelper( T (&array)[N] ))[N];

#define sizeof_array(array) (sizeof(_ArraySizeHelper(array)))

typedef glm::vec3 Vertex;

// Вершины куба служат материалом для формирования треугольников,
// составляющих грани куба.
const Vertex CUBE_VERTICIES[] = {
    {-1, +1, -1},
    {+1, +1, -1},
    {+1, -1, -1},
    {-1, -1, -1},
    {-1, +1, +1},
    {+1, +1, +1},
    {+1, -1, +1},
    {-1, -1, +1},
};

struct STriangleFace
{
    size_t vertexIndex1;
    size_t vertexIndex2;
    size_t vertexIndex3;
    size_t colorIndex;
};

// Привыкаем использовать 16-битный unsigned short,
// чтобы экономить память на фигурах с тысячами вершин.
const STriangleFace CUBE_FACES[] = {
    {0, 1, 2, static_cast<size_t>(CubeFace::Back)},
    {0, 2, 3, static_cast<size_t>(CubeFace::Back)},
    {2, 1, 5, static_cast<size_t>(CubeFace::Right)},
    {2, 5, 6, static_cast<size_t>(CubeFace::Right)},
    {3, 2, 6, static_cast<size_t>(CubeFace::Bottom)},
    {3, 6, 7, static_cast<size_t>(CubeFace::Bottom)},
    {0, 3, 7, static_cast<size_t>(CubeFace::Left)},
    {0, 7, 4, static_cast<size_t>(CubeFace::Left)},
    {1, 0, 4, static_cast<size_t>(CubeFace::Top)},
    {1, 4, 5, static_cast<size_t>(CubeFace::Top)},
    {6, 5, 4, static_cast<size_t>(CubeFace::Front)},
    {6, 4, 7, static_cast<size_t>(CubeFace::Front)},
};

/// @param phase - Фаза анимации на отрезке [0..1]
glm::mat4 GetRotateZTransfrom(float phase)
{
    // угол вращения вокруг оси Z лежит в отрезке [0...2*pi].
    const float angle = float(2 * M_PI) * phase;

    return glm::rotate(glm::mat4(), angle, {0, 0, 1});
}

/// @param phase - Фаза анимации на отрезке [0..1]
glm::mat4 GetScalingPulseTransform(float phase)
{
    // число пульсаций размера - произвольная константа.
    const int pulseCount = 4;
    float scale = fabsf(cosf(float(pulseCount * M_PI) * phase));

    return glm::scale(glm::mat4(), {scale, scale, scale});
}

/// @param phase - Фаза анимации на отрезке [0..1]
glm::mat4 GetBounceTransform(float phase)
{
    // начальная скорость и число отскоков - произвольные константы.
    const int bounceCount = 4;
    const float startSpeed = 15.f;
    // "время" пробегает bounceCount раз по отрезку [0...1/bounceCount].
    const float time = fmodf(phase, 1.f / float(bounceCount));
    // ускорение подбирается так, чтобы на 0.25с скорость стала
    // противоположна начальной.
    const float acceleration = - (startSpeed * 2.f * float(bounceCount));
    // расстояние - результат интегрирования функции скорости:
    //  speed = startSpeed + acceleration * time;
    float offset = time * (startSpeed + 0.5f * acceleration * time);

    // для отскоков с нечётным номером меняем знак.
    const int bounceNo = int(phase * bounceCount);
    if (bounceNo % 2)
    {
        offset = -offset;
    }

    return glm::translate(glm::mat4(), {offset, 0.f, 0.f});
}

}

CIdentityCube::CIdentityCube()
{
    // Используем белый цвет по умолчанию.
    for (glm::vec3 &color : m_colors)
    {
        color.x = 1;
        color.y = 1;
        color.z = 1;
    }
}

void CIdentityCube::Update(float deltaTime)
{
    (void)deltaTime;
}

void CIdentityCube::Draw() const
{
    // менее оптимальный способ рисования: прямая отправка данных
    // могла бы работать быстрее, чем множество вызовов glColor/glVertex.
    glBegin(GL_TRIANGLES);

    for (const STriangleFace &face : CUBE_FACES)
    {
        const Vertex &v1 = CUBE_VERTICIES[face.vertexIndex1];
        const Vertex &v2 = CUBE_VERTICIES[face.vertexIndex2];
        const Vertex &v3 = CUBE_VERTICIES[face.vertexIndex3];
        glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        glColor3fv(glm::value_ptr(m_colors[face.colorIndex]));
        glNormal3fv(glm::value_ptr(normal));
        glVertex3fv(glm::value_ptr(v1));
        glVertex3fv(glm::value_ptr(v2));
        glVertex3fv(glm::value_ptr(v3));
    }
    glEnd();
}

void CIdentityCube::SetFaceColor(CubeFace face, const glm::vec3 &color)
{
    const size_t index = static_cast<size_t>(face);
    assert(index < COLORS_COUNT);
    m_colors[index] = color;
}

const float CAnimatedCube::ANIMATION_STEP_SECONDS = 2.f;

void CAnimatedCube::Update(float deltaTime)
{
    // Вычисляем фазу анимации по времени на отрезке [0..1].
    m_animationPhase += (deltaTime / ANIMATION_STEP_SECONDS);
    if (m_animationPhase >= 1)
    {
        m_animationPhase = 0;
        switch (m_animation)
        {
        case Rotating:
            m_animation = Pulse;
            break;
        case Pulse:
            m_animation = Bounce;
            break;
        case Bounce:
            m_animation = Rotating;
            break;
        }
    }
}

void CAnimatedCube::Draw() const
{
    const glm::mat4 matrix = GetAnimationTransform();
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(matrix));
    CIdentityCube::Draw();
    glPopMatrix();
}

// Документация по функциям для модификации матриц:
// http://glm.g-truc.net/0.9.2/api/a00245.html
glm::mat4 CAnimatedCube::GetAnimationTransform() const
{
    switch (m_animation)
    {
    case Rotating:
        return GetRotateZTransfrom(m_animationPhase);
    case Pulse:
        return GetScalingPulseTransform(m_animationPhase);
    case Bounce:
        return GetBounceTransform(m_animationPhase);
    }
    // Недостижимый код - вернём единичную матрицу.
    return glm::mat4();
}
