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
    void OnKeyDown(const SDL_KeyboardEvent &) override;
    void OnKeyUp(const SDL_KeyboardEvent &) override;

private:
    void CheckOpenGLVersion();
    void SetupView(const glm::ivec2 &size);

    CTexture2DUniquePtr m_pEarthTexture;
    CTexture2DUniquePtr m_pCloudTexture;
    CIdentitySphere m_sphereObj;
    CCamera m_camera;
    CPhongModelMaterial m_sphereMat;
    CDirectedLightSource m_sunlight;
    CShaderProgram m_programPhong;
    CShaderProgram m_programFixed;
    std::vector<CShaderProgram *> m_programQueue;
};
