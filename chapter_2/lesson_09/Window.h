#pragma once
#include "libchapter2.h"
#include "IBody.h"
#include "Lights.h"
#include <vector>

class CWindow : public CAbstractInputControlWindow
{
public:
    CWindow();

protected:
    // CAbstractWindow interface
    void OnWindowInit(const glm::ivec2 &size) override;
    void OnUpdateWindow(float deltaSeconds) override;
    void OnDrawWindow(const glm::ivec2 &size) override;

    // IInputEventAcceptor interface
    void OnKeyDown(const SDL_KeyboardEvent &) override;
    void OnKeyUp(const SDL_KeyboardEvent &) override;

private:
    void InitBodies();
    void SetupView(const glm::ivec2 &size);

    std::vector<IBodyUniquePtr> m_opaqueBodies;
    std::vector<IBodyUniquePtr> m_transparentBodies;
    CCamera m_camera;
    CDirectedLightSource m_sunlight;
};
