#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL_events.h>
#include <boost/noncopyable.hpp>
#include <set>

class CLightingSystem
{
public:
    CLightingSystem() = delete;

    static void Enable();
    static void Disable();
    static void SetGlobalAmbient(const glm::vec3 &color);
    static void SetTwoSideLightingEnabled(bool enabled);
};

class ILightSource
{
public:
    virtual ~ILightSource() = default;
    virtual void Setup()const = 0;
};

class CAbstractLightSource : public ILightSource
{
public:
    /// @param index - один из GL_LIGHT*
    CAbstractLightSource(unsigned index);
    ~CAbstractLightSource();

    glm::vec4 GetAmbient() const;
    glm::vec4 GetDiffuse() const;
    glm::vec4 GetSpecular() const;
    void SetAmbient(const glm::vec4 &color);
    void SetDiffuse(const glm::vec4 &color);
    void SetSpecular(const glm::vec4 &color);

protected:
    void RenderImpl()const;
    unsigned GetIndex()const;

private:
    glm::vec4 m_ambient;
    glm::vec4 m_diffuse;
    glm::vec4 m_specular;
    unsigned m_index;
};

class CDirectedLightSource : public CAbstractLightSource
{
public:
    /// @param index - один из GL_LIGHT*
    CDirectedLightSource(unsigned index);

    glm::vec3 GetDirection() const;
    void SetDirection(const glm::vec3 &value);

    void Setup() const override;

private:
    glm::vec4 m_direction;
};

class CPositionLightSource : public CAbstractLightSource
{
public:
    /// @param index - один из GL_LIGHT*
    CPositionLightSource(unsigned index);

    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3 &value);

    void Setup() const override;

private:
    glm::vec4 m_position;
};
