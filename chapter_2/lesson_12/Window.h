#pragma once
#include "libchapter2.h"
#include "Lights.h"
#include "MemoryField.h"
#include "HeadUpDisplay.h"
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
    void OnDragEnd(const glm::vec2 &pos) override;

private:
    void SetupView(const glm::ivec2 &size);
    glm::mat4 GetProjectionMatrix(const glm::ivec2 &size);
    void ShowGameOverMessage();

    std::unique_ptr<CMemoryField> m_pField;
    std::unique_ptr<CHeadUpDisplay> m_pHud;
    CCamera m_camera;
    CDirectedLightSource m_sunlight;
};
