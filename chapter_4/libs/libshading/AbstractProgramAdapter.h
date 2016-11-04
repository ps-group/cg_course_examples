#pragma once

#include "IProgramAdapter.h"
#include <unordered_map>

class CShaderProgram;

class CAbstractProgramAdapter : public IProgramAdapter
{
public:
    void Use() const override;
    CProgramUniform GetUniform(UniformId id) const override;
    CVertexAttribute GetAttribute(AttributeId id) const override;

protected:
    using UniformNamesMap = std::unordered_map<UniformId, std::string>;
    using AttributeNamesMap = std::unordered_map<AttributeId, std::string>;

    virtual const CShaderProgram &GetProgram()const = 0;

    void SetUniformNames(const UniformNamesMap &names);
    void SetAttributeNames(const AttributeNamesMap &names);

private:
    UniformNamesMap m_uniformNames;
    AttributeNamesMap m_attributeNames;
};
