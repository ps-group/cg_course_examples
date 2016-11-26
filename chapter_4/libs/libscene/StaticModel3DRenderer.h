#pragma once
#include <glm/mat4x4.hpp>

class CStaticMesh3D;
class CStaticModel3D;
class IProgramAdapter;
class CProgramUniform;
struct SPhongMaterial;
struct SGeometryLayout;
enum class UniformId : unsigned;

class CStaticModel3DRenderer
{
public:
    void SetWorldMat4(const glm::mat4 &value);
    void SetViewMat4(const glm::mat4 &value);
    void SetProjectionMat4(const glm::mat4 &value);
    void Use(IProgramAdapter &program);

    void Draw(CStaticModel3D &model);

private:
    CProgramUniform GetUniform(UniformId id)const;
    void ApplyModelView(const glm::mat4 &local);
    void ApplyMaterial(const SPhongMaterial &material)const;
    void BindAttributes(const SGeometryLayout &layout)const;

    IProgramAdapter *m_pProgram = nullptr;
    glm::mat4 m_world;
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_normal;
};
