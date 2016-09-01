#pragma once
#include "libchapter3.h"
#include "WhitneyUmbrella.h"
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
    void SetupView(const glm::ivec2 &size);

    CWhitneyUmbrella m_umbrellaObj;
    CPhongModelMaterial m_umbrellaMat;
    CCamera m_camera;
    CDirectedLightSource m_sunlight;
    CPositionLightSource m_lamp;
    CShaderProgram m_programTwist;
    CShaderProgram m_programFixed;
    bool m_programEnabled = true;
    float m_currentTwistValue = 0;
    float m_nextTwistValue = 0;
};
