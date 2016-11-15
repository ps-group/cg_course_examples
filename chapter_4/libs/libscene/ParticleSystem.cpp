#include "ParticleSystem.h"
#include "libshading/IProgramAdapter.h"
#include "libshading/VertexAttribute.h"
#include "libshading/ProgramUniform.h"
#include "includes/opengl-common.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace
{
// Двумерные координаты двух треугольников, задающих квадратный спрайт.
vec2 SPRITE_VERTECIES[] = {
    { -1.f, -1.f }, // вершина 3
    { +1.f, -1.f }, // вершина 2
    { -1.f, +1.f }, // вершина 0
    { -1.f, +1.f }, // вершина 0
    { +1.f, -1.f }, // вершина 2
    { +1.f, +1.f }, // вершина 1
};

const size_t SPRITE_VERTEX_COUNT = sizeof(SPRITE_VERTECIES) / sizeof(SPRITE_VERTECIES[0]);
}

// Конструирует систему частиц с двумя буферами данных:
//  один используется для статичного спрайта (текстурированного прямоугольника),
//  второй - для постоянно изменяющихся позиций частиц
// в связи с этим у них разные константы BufferUsage.
CParticleSystem::CParticleSystem()
    : m_spriteGeometry(BufferType::Attributes, BufferUsage::StaticDraw)
    , m_particlePositions(BufferType::Attributes, BufferUsage::StreamDraw)
{
    m_spriteGeometry.Copy(SPRITE_VERTECIES, sizeof(SPRITE_VERTECIES));
}

CParticleSystem::~CParticleSystem()
{
}

void CParticleSystem::SetEmitter(std::unique_ptr<CParticleEmitter> &&pEmitter)
{
    m_pEmitter = std::move(pEmitter);
}

void CParticleSystem::SetGravity(const vec3 &gravity)
{
    m_gravity = gravity;
}

void CParticleSystem::SetParticleTexture(const CTexture2DSharedPtr &pTexture)
{
    m_pTexture = pTexture;
}

void CParticleSystem::Advance(float dt)
{
    // Генерируем новые частицы (за 1 кадр может появиться несколько частиц).
    m_pEmitter->Advance(dt);
    while (m_pEmitter->IsEmitReady())
    {
        m_particles.emplace_back(m_pEmitter->Emit());
    }

    // Продвигаем время жизни всех частиц.
    for (auto &particle : m_particles)
    {
        particle.Advance(dt, m_gravity);
    }

    // Удаляем "умершие" частицы.
    auto newEnd = std::remove_if(m_particles.begin(), m_particles.end(),
                                 [](const auto &particle) {
        return !particle.IsAlive();
    });
    m_particles.erase(newEnd, m_particles.end());

    // Устанавливаем флаг, указывающий, что позиции частиц в видеопамяти
    //  следует обновить.
    m_isDirty = true;
}

void CParticleSystem::Draw(IProgramAdapter &program, const glm::mat4 &worldView)
{
    if (!m_pTexture)
    {
        throw std::runtime_error("No texture set for particle system");
    }
    m_pTexture->Bind();

    // Обновляем и привязываем буфер с позициями частиц
    BindParticlePositions(program, worldView);

    // Привязываем буфер с вершинами прямоугольного спрайта.
    m_spriteGeometry.Bind();
    CVertexAttribute texCoordAttr = program.GetAttribute(AttributeId::TEX_COORD_UV);
    texCoordAttr.SetVec2Offset(0, sizeof(vec2));

    const GLsizei vertexCount = GLsizei(SPRITE_VERTEX_COUNT);
    const GLsizei instanceCount = GLsizei(m_particles.size());
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, instanceCount);
}

// Привязывает буфер с позициями частиц,
//  при необходимости обновляет его,
//  затем связывает с атрибутом шейдера.
void CParticleSystem::BindParticlePositions(IProgramAdapter &program, const glm::mat4 &worldView)
{
    m_particlePositions.Bind();

    if (m_isDirty)
    {
        UpdateParticlePositions(worldView);
        m_isDirty = false;
    }

    CVertexAttribute positionAttr = program.GetAttribute(AttributeId::INSTANCE_POSITION);
    positionAttr.SetVec3Offset(0, sizeof(vec3), false);
}

void CParticleSystem::UpdateParticlePositions(const glm::mat4 &worldView)
{
    // Собираем массив позиций частиц
    std::vector<vec3> positions(m_particles.size());
    std::transform(m_particles.begin(), m_particles.end(),
                   positions.begin(), [](const CParticle &particle) {
        return particle.GetPosition();
    });

    // Сортируем частицы в порядке удалённости от камеры
    std::sort(positions.begin(), positions.end(), [&](const vec3 &a, vec3 &b) {
        const vec3 viewA = vec3(worldView * vec4(a, 1.0));
        const vec3 viewB = vec3(worldView * vec4(b, 1.0));
        return viewA.z < viewB.z;
    });

    // Отправляем данные на видеокарту
    m_particlePositions.Copy(positions);
}
