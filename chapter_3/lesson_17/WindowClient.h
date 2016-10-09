#pragma once
#include "libchapter3.h"
#include "IdentitySphere.h"
#include "EarthProgramContext.h"
#include <vector>

class CWindowClientBase
        : public CAbstractWindowClient
{
public:
    CWindowClientBase(CWindow &window)
        : CAbstractWindowClient(window)
    {
        // Прикрепляем VAO, который будет объектом по-умолчанию.
        //  http://stackoverflow.com/questions/13403807/
        m_defaultVAO.Bind();
    }

private:
    CArrayObject m_defaultVAO;
};

class CWindowClient
        : public CWindowClientBase
{
public:
    CWindowClient(CWindow &window);

protected:
    // IWindowClient interface
    void OnUpdateWindow(float deltaSeconds) override;
    void OnKeyDown(const SDL_KeyboardEvent &) override;
    void OnKeyUp(const SDL_KeyboardEvent &) override;

private:
    void CheckOpenGLVersion();
    void UpdateRotation(float deltaSeconds);
    void SetupView(const glm::ivec2 &size);
    void SetupLight0();

    CIdentitySphere m_sphereObj;
    CCamera m_camera;
    CDirectedLightSource m_sunlight;
    CEarthProgramContext m_programContext;
};
