#include "stdafx.h"
#include "ScriptSystem.h"


CScriptSystem::CScriptSystem()
{
}

void CScriptSystem::Update(float deltaSeconds)
{
    for (const auto &entity : getEntities())
    {
        auto &script = entity.getComponent<CScriptComponent>();
        script.m_onUpdate(deltaSeconds, entity);
    }
}
