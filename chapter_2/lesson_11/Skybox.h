#pragma once

#include "IdentityCube.h"
#include "libchapter2.h"
#include <boost/noncopyable.hpp>

class CSkybox final : public ISceneObject
{
public:
    CSkybox();
    void Update(float dt) final;
    void Draw()const final;

private:
    CIdentityCube m_cube;
    CTexture2DAtlas m_atlas;
};
