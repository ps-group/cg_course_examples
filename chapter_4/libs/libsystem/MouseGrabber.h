#pragma once
#include <vector>
#include "includes/glm-common.hpp"

class CWindow;
struct SDL_MouseMotionEvent;

// Класс захватывает курсор мыши, а также решает проблему
//  перемещения невидимого курсора к границам окна:
//  - на каждом кадре курсор принудительно перемещается в центр окна
//  - перемещение курсора порождает событие mouse moved
//    с известными deltaX, deltaY, что позволяет
//    игнорировать это событие.
// Можно заподозрить, что иногда mouse grabber будет фильтровать
//  не те события и тем самым нарушать правильную обработку мыши,
//  но это неправда: общее движение мыши остаётся инвариантом.
class CMouseGrabber
{
public:
    CMouseGrabber(CWindow &window);

    // Возвращает true, если событие было поглощено.
    // Событие поглощается, если оно порождено программным
    //  перемещением мыши.
    bool OnMouseMotion(const SDL_MouseMotionEvent &event);

private:
    void WarpMouseSafely();

    CWindow &m_windowRef;

    // Ожидаемые координаты событий mouse move, порождаемых
    //  программным перемещением курсора,
    //  события с такими координатами игнорируются.
    std::vector<glm::ivec2> m_blacklistedMoves;
};
