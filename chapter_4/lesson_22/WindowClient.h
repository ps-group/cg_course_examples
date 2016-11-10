#pragma once
#include "libsystem/AbstractWindowClient.h"
#include "libsystem/MouseGrabber.h"
#include "libscene/ArrayObject.h"
#include "libscene/Camera.h"
#include "SceneLoader.h"
#include "RenderSystem.h"
#include <vector>

class CWindowClient
        : public CAbstractWindowClient
{
public:
    CWindowClient(CWindow &window);

protected:
    // IWindowClient interface
    void OnUpdate(float deltaSeconds) override;
    void OnDraw() override;
    bool OnKeyDown(const SDL_KeyboardEvent &) override;
    bool OnKeyUp(const SDL_KeyboardEvent &) override;
    bool OnMousePress(const SDL_MouseButtonEvent &) override;
    bool OnMouseMotion(const SDL_MouseMotionEvent &) override;
    bool OnMouseUp(const SDL_MouseButtonEvent &) override;

private:
    // Данный VAO будет объектом по-умолчанию.
    // Его привязка должна произойти до первой привязки VBO.
    //  http://stackoverflow.com/questions/13403807/
    CArrayObject m_defaultVAO;
    anax::World m_world;
    CRenderSystem m_renderSystem;

    CMouseGrabber m_mouseGrabber;
    CCamera m_camera;
};
