#pragma once
#include "libchapter3.h"
#include "IdentitySphere.h"
#include <vector>

class CWindowClient : public CAbstractWindowClient
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
    void SetupView(const glm::ivec2 &size);

    CIdentitySphere m_sphereObj;
    CPhongModelMaterial m_sphereMat;
    CCamera m_camera;
    CDirectedLightSource m_sunlight;
    CShaderProgram m_programPhong;
    CShaderProgram m_programLambert;
    CShaderProgram m_programFixed;
    std::vector<CShaderProgram *> m_programQueue;
};
