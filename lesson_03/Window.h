#pragma once
#include "DispatchEvent.h"

class CWindow : public CAbstractInputControlWindow
{
public:
    CWindow();

    // CAbstractWindow interface
protected:
    void OnUpdateWindow(float deltaSeconds) override;
    void OnDrawWindow(const glm::ivec2 &size) override;

private:
    void SetupView(const glm::ivec2 &size);
    void DrawShapes();

    float m_time = 0.f;
};
