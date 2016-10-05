#pragma once

#include "libchapter2.h"
#include <GL/glu.h>
#include <boost/noncopyable.hpp>

enum class CubeFace
{
    Front = 0,
    Back,
    Top,
    Bottom,
    Left,
    Right,

    NumFaces
};

class CIdentityCube final : public ISceneObject
{
public:
    CIdentityCube();
    void Update(float deltaTime) final;
    void Draw()const final;

    void SetFaceColor(CubeFace face, const glm::vec3 &color);
    void SetAlpha(float alpha);

private:
    void OutputFaces()const;

    static const size_t COLORS_COUNT = static_cast<size_t>(CubeFace::NumFaces);
    glm::vec3 m_colors[COLORS_COUNT];
    float m_alpha;
};

class CIdentityTetrahedron final : public ISceneObject
{
public:
    void Update(float deltaTime) final;
    void Draw()const final;

    void SetColor(const glm::vec4 &color);

private:
    void OutputFaces()const;

    glm::vec4 m_color;
};

class CSphereQuadric final
        : public ISceneObject
        , private boost::noncopyable
{
public:
    CSphereQuadric();
    ~CSphereQuadric();

    void Update(float) final {}
    void Draw()const final;

    void SetColor(const glm::vec3 &color);

private:
    GLUquadric *m_quadric = nullptr;
    glm::vec3 m_color;
};

class CConoidQuadric final
        : public ISceneObject
        , private boost::noncopyable
{
public:
    CConoidQuadric();
    ~CConoidQuadric();

    void Update(float) final {}
    void Draw()const final;

    /// @param value - in range [0..1]
    void SetTopRadius(double value);
    void SetColor(const glm::vec3 &color);

private:
    GLUquadric *m_quadric = nullptr;
    double m_topRadius = 1.;
    glm::vec3 m_color;
};
