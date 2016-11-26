#pragma once

#include "IProgramAdapter.h"
#include <unordered_map>


// FIXME: исправление для компиляторов с неполной поддержкой C++14.
// В стандарте C++11 функция std::hash для enum не была определена,
//  в C++14 эту проблему исправили,
//  но в libstdc++ версии 5.4 исправление ещё не реализовано
// http://stackoverflow.com/questions/18837857
#ifndef _MSC_VER // в VS2015 и выше проблема не проявляется.
namespace std {
  template <> struct hash<UniformId> {
    size_t operator() (const UniformId &t) const noexcept { return size_t(t); }
  };
template <> struct hash<AttributeId> {
  size_t operator() (const AttributeId &t) const noexcept { return size_t(t); }
};
}
#endif


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
