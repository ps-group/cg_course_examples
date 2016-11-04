#pragma once

#include "libsystem/AbstractWindowClient.h"
#include "libsystem/MouseGrabber.h"
#include "libscene/ArrayObject.h"
#include "libscene/Camera.h"
#include "SceneLoader.h"
#include "RenderSystem.h"
#include "TimeController.h"
#include "KeplerLawSystem.h"
#include "BodyRotationSystem.h"
#include <vector>

class CWindowClient : public CAbstractWindowClient
{
public:
    CWindowClient(CWindow &window);

protected:
    // IWindowClient interface
    void OnUpdate(float deltaSeconds) override;
    void OnDraw() override;
    bool OnKeyDown(const SDL_KeyboardEvent &) override;
    bool OnKeyUp(const SDL_KeyboardEvent &) override;
    bool OnMousePress(const SDL_MouseButtonEvent &event) override;
    bool OnMouseMotion(const SDL_MouseMotionEvent &event) override;
    bool OnMouseUp(const SDL_MouseButtonEvent &event) override;

private:
    // Данный VAO будет объектом по-умолчанию.
    // Его привязка должна произойти до первой привязки VBO.
    //  http://stackoverflow.com/questions/13403807/
    CArrayObject m_defaultVAO;
    anax::World m_world;
    CTimeController m_timeController;
    CRenderSystem m_renderSystem;
    CKeplerLawSystem m_keplerSystem;
    CBodyRotationSystem m_rotationSystem;

    CMouseGrabber m_mouseGrabber;
    CCamera m_camera;
};
