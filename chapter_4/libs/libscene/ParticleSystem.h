#pragma once

#include <memory>
#include "includes/glm-common.hpp"
#include "ParticleEmitter.h"
#include "BufferObject.h"
#include "Texture2D.h"

class IProgramAdapter;

class CParticleSystem : private boost::noncopyable
{
public:
    CParticleSystem();
    ~CParticleSystem();

    void SetEmitter(std::unique_ptr<CParticleEmitter> && pEmitter);
    void SetGravity(const glm::vec3 &gravity);
    void SetParticleTexture(const CTexture2DSharedPtr &pTexture);

    // @param dt - разница во времени с предыдущим вызовом Advance.
    void Advance(float dt);

    // @param program - должна быть активирована перед рисованием.
    // @param worldView - задаёт преобразование из локальных координат
    //   системы частиц в систему координат камеры.
    void Draw(IProgramAdapter &program, const glm::mat4 &worldView);

private:
    void BindParticlePositions(IProgramAdapter &program, const glm::mat4 &worldView);
    void UpdateParticlePositions(const glm::mat4 &worldView);

    std::unique_ptr<CParticleEmitter> m_pEmitter;
    std::vector<CParticle> m_particles;
    glm::vec3 m_gravity;

    // Данные для рисования
    bool m_isDirty = false;
    CTexture2DSharedPtr m_pTexture;
    CBufferObject m_spriteGeometry;
    CBufferObject m_particlePositions;
};
