#pragma once
#include "libchapter3.h"
#include "IdentitySphere.h"
#include "EarthProgramContext.h"
#include <vector>

class CWindowClient
        : public CAbstractWindowClient
{
public:
    CWindowClient(CWindow &window);

protected:
    // IWindowClient interface
    void OnUpdateWindow(float deltaSeconds) override;
    void OnDrawWindow() override;
    void OnKeyDown(const SDL_KeyboardEvent &) override;
    void OnKeyUp(const SDL_KeyboardEvent &) override;

private:
    void CheckOpenGLVersion();
    void UpdateRotation(float deltaSeconds);
    void SetupView(const glm::ivec2 &size);

    CIdentitySphere m_sphereObj;
    CCamera m_camera;
    CPhongModelMaterial m_sphereMat;
    CDirectedLightSource m_sunlight;
    CEarthProgramContext m_programContext;
    glm::mat4 m_earthTransform;
};
