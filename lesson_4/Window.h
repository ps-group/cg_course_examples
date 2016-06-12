#pragma once
#include "DispatchEvent.h"
#include "MovableFlower.h"
#include <vector>

class CWindow : public CAbstractInputControlWindow
{
public:
    CWindow();

protected:
    // CAbstractWindow interface
    void OnUpdateWindow(float deltaSeconds) override;
    void OnDrawWindow(const glm::ivec2 &size) override;

    // IInputEventAcceptor interface
    void OnDragBegin(const glm::vec2 &pos) override;
    void OnDragMotion(const glm::vec2 &pos) override;
    void OnDragEnd(const glm::vec2 &pos) override;

private:
    void SetupView(const glm::ivec2 &size);

    std::vector<std::unique_ptr<CMovableFlower>> m_flowers;
    CMovableFlower *m_draggingFlower = nullptr;
    glm::vec2 m_dragOffset;
};
