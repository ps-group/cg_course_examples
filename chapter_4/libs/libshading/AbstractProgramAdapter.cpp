#include "AbstractProgramAdapter.h"
#include "ProgramUniform.h"
#include "VertexAttribute.h"
#include "ShaderProgram.h"

void CAbstractProgramAdapter::Use() const
{
    GetProgram().Use();
}

CProgramUniform CAbstractProgramAdapter::GetUniform(UniformId id) const
{
    const auto it = m_uniformNames.find(id);
    if (it != m_uniformNames.end())
    {
        return GetProgram().FindUniform(it->second);
    }
    return CProgramUniform(-1);
}

CVertexAttribute CAbstractProgramAdapter::GetAttribute(AttributeId id) const
{
    const auto it = m_attributeNames.find(id);
    if (it != m_attributeNames.end())
    {
        return GetProgram().FindAttribute(it->second);
    }
    return CVertexAttribute(-1);
}

void CAbstractProgramAdapter::SetUniformNames(const UniformNamesMap &names)
{
    m_uniformNames = names;
}

void CAbstractProgramAdapter::SetAttributeNames(const AttributeNamesMap &names)
{
    m_attributeNames = names;
}
