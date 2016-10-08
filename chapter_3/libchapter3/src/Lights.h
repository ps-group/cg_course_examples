#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <boost/noncopyable.hpp>
#include <set>

class ILightSource
{
public:
    virtual ~ILightSource() = default;
    virtual void Setup()const = 0;

    virtual glm::vec4 GetUniformPosition() const = 0;
    virtual glm::vec4 GetAmbient() const = 0;
    virtual glm::vec4 GetDiffuse() const = 0;
    virtual glm::vec4 GetSpecular() const = 0;
    virtual void SetAmbient(const glm::vec4 &color) = 0;
    virtual void SetDiffuse(const glm::vec4 &color) = 0;
    virtual void SetSpecular(const glm::vec4 &color) = 0;
};

class CAbstractLightSource
        : public ILightSource
        , private boost::noncopyable
{
public:
    /// @param index - один из GL_LIGHT*
    CAbstractLightSource(unsigned index);
    ~CAbstractLightSource();

    glm::vec4 GetUniformPosition()const final;
    glm::vec4 GetAmbient() const final;
    glm::vec4 GetDiffuse() const final;
    glm::vec4 GetSpecular() const final;
    void SetAmbient(const glm::vec4 &color) final;
    void SetDiffuse(const glm::vec4 &color) final;
    void SetSpecular(const glm::vec4 &color) final;

protected:
    void SetUniformPosition(const glm::vec4 &position);
    void SetupImpl()const;
    unsigned GetIndex()const;

private:
    glm::vec4 m_uniformPosition;
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
};

class CPositionLightSource : public CAbstractLightSource
{
public:
    /// @param index - один из GL_LIGHT*
    CPositionLightSource(unsigned index);

    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3 &value);

    void Setup() const override;
};

class CPhongModelMaterial
{
public:
    void Setup() const;

    glm::vec4 GetEmission() const;
    glm::vec4 GetAmbient() const;
    glm::vec4 GetDiffuse() const;
    glm::vec4 GetSpecular() const;
    float GetShininess() const;

    void SetAmbient(const glm::vec4 &GetAmbient);
    void SetEmission(const glm::vec4 &GetEmission);
    void SetDiffuse(const glm::vec4 &GetDiffuse);
    void SetSpecular(const glm::vec4 &GetSpecular);
    void SetShininess(float GetShininess);

private:
    glm::vec4 m_emission;
    glm::vec4 m_ambient;
    glm::vec4 m_diffuse;
    glm::vec4 m_specular;
    float m_shininess = 0.f;
};
