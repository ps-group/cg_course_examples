#pragma once
#include "DispatchEvent.h"
#include "ParticleSystem.h"
#include <vector>

class CWindow : public CAbstractInputControlWindow
{
public:
    CWindow();

protected:
    // CAbstractWindow interface
    void OnUpdateWindow(float deltaSeconds) override;
    void OnDrawWindow(const glm::ivec2 &size) override;

private:
    void SetupView(const glm::ivec2 &size);

    CParticleSystem m_system;
};
