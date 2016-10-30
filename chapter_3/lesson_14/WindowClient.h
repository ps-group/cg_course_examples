#pragma once
#include "libchapter3.h"
#include "Quad.h"
#include <vector>

class CWindowClient : public CAbstractWindowClient
{
public:
    CWindowClient(CWindow &window);

protected:
    // IWindowClient interface
    void OnUpdateWindow(float deltaSeconds) override;
    void OnDrawWindow() override;
    void OnKeyUp(const SDL_KeyboardEvent &) override;

private:
    void CheckOpenGLVersion();
    void SetupView(const glm::ivec2 &size);

    CQuad m_quadObj;
    CShaderProgram m_programCheckers;
    CShaderProgram m_programPicture;
    std::vector<CShaderProgram *> m_programQueue;
};
