#pragma once

#include "Window.h"
#include "IInputListener.h"

// Обрабатывает обновление состояния и рисование кадра
// Также обрабатывает события SDL, разделённые по категориям.
class IWindowClient : public IInputListener
{
public:
    virtual ~IWindowClient() = default;

    virtual void OnUpdate(float deltaSeconds) = 0;
    virtual void OnDraw() = 0;
};

class CWindow;

class CAbstractWindowClient : IWindowClient
{
public:
    CAbstractWindowClient(CWindow &window);
    ~CAbstractWindowClient();

protected:
    CWindow &GetWindow();
    const CWindow &GetWindow()const;

private:
    CWindow &m_window;
};
