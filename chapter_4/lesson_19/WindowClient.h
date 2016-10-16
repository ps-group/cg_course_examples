#pragma once
#include "libchapter4.h"
#include "Tesselator.h"
#include "SceneLoader.h"
#include "RenderSystem.h"
#include "TimeController.h"
#include "KeplerLawSystem.h"
#include "BodyRotationSystem.h"
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
    void OnKeyDown(const SDL_KeyboardEvent &) override;
    void OnKeyUp(const SDL_KeyboardEvent &) override;

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

    CCamera m_camera;
};
