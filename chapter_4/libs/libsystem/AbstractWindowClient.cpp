#include "includes/sdl-common.hpp"
#include "AbstractWindowClient.h"

CAbstractWindowClient::CAbstractWindowClient(CWindow &window)
    : m_window(window)
{
    m_window.SetClient(this);
}

CAbstractWindowClient::~CAbstractWindowClient()
{
    m_window.SetClient(nullptr);
}

CWindow &CAbstractWindowClient::GetWindow()
{
    return m_window;
}

const CWindow &CAbstractWindowClient::GetWindow() const
{
    return m_window;
}
