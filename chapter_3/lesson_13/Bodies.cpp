#include "stdafx.h"
#include "Bodies.h"

namespace
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
};

const glm::vec3 DARK_GREEN = {0.05f, 0.45f, 0.1f};
const glm::vec3 LIGHT_GREEN = {0.1f, 0.8f, 0.15f};

// Вершины куба служат материалом для формирования треугольников,
// составляющих грани куба.
const Vertex CUBE_VERTICIES[] = {
    {{-1, +1, -1}, DARK_GREEN},
    {{+1, +1, -1}, DARK_GREEN},
    {{+1, -1, -1}, DARK_GREEN},
    {{-1, -1, -1}, DARK_GREEN},
    {{-1, +1, +1}, LIGHT_GREEN},
    {{+1, +1, +1}, LIGHT_GREEN},
    {{+1, -1, +1}, LIGHT_GREEN},
    {{-1, -1, +1}, LIGHT_GREEN},
};

// Привыкаем использовать 16-битный unsigned short,
// чтобы экономить память на фигурах с тысячами вершин.
const uint16_t CUBE_FACES[] = {
    0, 1, 2,
    0, 2, 3,
    2, 1, 5,
    2, 5, 6,
    3, 2, 6,
    3, 6, 7,
    0, 3, 7,
    0, 7, 4,
    1, 0, 4,
    1, 4, 5,
    6, 5, 4,
    6, 4, 7,
};

}

void CIdentityCube::Update(float deltaTime)
{
    (void)deltaTime;
}

void CIdentityCube::Draw()
{
    // менее оптимальный способ рисования: прямая отправка данных
    // могла бы работать быстрее, чем множество вызовов glColor/glVertex.
    glBegin(GL_TRIANGLES);
    for (uint16_t i : CUBE_FACES)
    {
        const Vertex &v = CUBE_VERTICIES[i];
        glColor3f(v.color.x, v.color.y, v.color.z);
        glVertex3f(v.pos.x, v.pos.y, v.pos.z);
    }
    glEnd();
}
