#pragma once
#include "libchapter4.h"
#include "Tesselator.h"
#include "StaticRenderSystem.h"
#include <vector>
#include <anax/World.hpp>

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
    void InstantiateEntities();
    void SetupSystems();

    // Данный VAO будет объектом по-умолчанию.
    // Его привязка должна произойти до первой привязки VBO.
    //  http://stackoverflow.com/questions/13403807/
    CArrayObject m_defaultVAO;
    anax::World m_world;
    CStaticRenderSystem m_renderSystem;

    CCamera m_camera;
};
