#include "stdafx.h"
#include "KeplerLawSystem.h"
#include "TimeController.h"
#include "Scale.h"
#include <unordered_set>
#include <unordered_map>
#include <boost/algorithm/cxx11/copy_if.hpp>

using glm::vec2;
using glm::vec3;

CKeplerLawSystem::CKeplerLawSystem(ITimeController &controller)
    : m_timeController(controller)
{
}

void CKeplerLawSystem::Update()
{
    const double time = m_timeController.GetSpaceTime();

    // Имена объектов, орбиты которых ещё следует обновить.
    // Используются для разрешения зависимостей при систем
    //  типа Солнце->Земля->Луна.
    std::unordered_set<std::string> remainingNames;
    for (const auto &entity : getEntities())
    {
        const auto &body = entity.getComponent<CSpaceBodyComponent>();
        remainingNames.insert(body.m_name);
    }

    // Словарь с известным позициями объектов,
    //  используется для расчёта позиций спутников.
    std::unordered_map<std::string, vec3> knownPositions;

    // Прекращаем обновлять позиции, если ничего не изменилось
    //  за один обход всех космических тел.
    bool mayContinue = false;
    do
    {
        mayContinue = false;
        for (const auto &entity : getEntities())
        {
            const auto &body = entity.getComponent<CSpaceBodyComponent>();
            auto &orbit = entity.getComponent<CEllipticOrbitComponent>();

            // Пропускаем итерацию, если спутник уже обновлён,
            //  либо не обновлён владелец спутника.
            if (!remainingNames.count(body.m_name) || remainingNames.count(orbit.m_ownerName))
            {
                continue;
            }
            mayContinue = true;

            const vec2 pos2D = orbit.PlanetPosition2D(time);
            const vec3 pos3D = scale::AU_SIZE * vec3(pos2D.x, 0.f, pos2D.y);
            const vec3 ownerPos3D = knownPositions[orbit.m_ownerName];

            auto &transform = entity.getComponent<CTransformComponent>();
            transform.m_position = pos3D + ownerPos3D;

            knownPositions[body.m_name] = transform.m_position;
            remainingNames.erase(body.m_name);
        }
    }
    while (mayContinue);

    // Если ещё есть необновлённые объекты, значит,
    //  есть циклические зависимости.
    if (!remainingNames.empty())
    {
        throw std::runtime_error("Cannot resolve orbit dependency cycle");
    }
}
