#pragma once
#include <glm/mat4x4.hpp>
#include <unordered_map>

class CSkeletalNode;
class CSkeletalMesh3D;
class CSkeletalModel3D;
class IProgramAdapter;
class CProgramUniform;
struct SPhongMaterial;
struct SGeometryLayout;
enum class UniformId : unsigned;

class CSkeletalModel3DRenderer
{
public:
    void SetWorldMat4(const glm::mat4 &value);
    void SetViewMat4(const glm::mat4 &value);
    void SetProjectionMat4(const glm::mat4 &value);
    void Use(IProgramAdapter &program);

    void Draw(CSkeletalModel3D &model);

private:
    CProgramUniform GetUniform(UniformId id)const;
    void SetupTransforms();
    void UpdateNodeTransformsCache(const CSkeletalModel3D &model);
    void SetupBoneTransforms(const CSkeletalMesh3D &mesh);
    void VisitNode(const CSkeletalNode &node,
                   const glm::mat4 &parentMat4);
    void ApplyMaterial(const SPhongMaterial &material)const;
    void BindAttributes(const SGeometryLayout &layout)const;

    IProgramAdapter *m_pProgram = nullptr;
    glm::mat4 m_world;
    glm::mat4 m_view;
    glm::mat4 m_projection;
    glm::mat4 m_normal;
    std::unordered_map<std::string, glm::mat4> m_nodeTransformsCache;
};
