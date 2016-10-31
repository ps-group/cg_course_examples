#include "stdafx.h"
#include "Tesselator.h"
#include "MeshP3NT2.h"
#include <algorithm>

namespace
{

struct CSphereTesselator : SMeshDataP3NT2
{
public:
    void Tesselate(unsigned slices, unsigned stacks)
    {
        MakeVertexAttributes(slices, stacks);
        MakeTriangleStripIndicies(slices, stacks);
    }

private:
    void MakeVertexAttributes(unsigned columnCount, unsigned rowCount)
    {
        // вычисляем позиции вершин.
        for (unsigned ci = 0; ci < columnCount; ++ci)
        {
            const float u = float(ci) / float(columnCount - 1);
            for (unsigned ri = 0; ri < rowCount; ++ri)
            {
                const float v = float(ri) / float(rowCount - 1);

                SVertexP3NT2 vertex;
                vertex.position = GetPositionOnSphere(u, v);

                // Нормаль к сфере - это нормализованный вектор радиуса к данной точке
                // Поскольку координаты центра равны 0, координаты вектора радиуса
                // будут равны координатам вершины.
                // Благодаря радиусу, равному 1, нормализация не требуется.
                vertex.normal = vertex.position;

                // Обе текстурные координаты должны плавно изменяться от 0 до 1,
                // натягивая прямоугольную картинку на тело вращения.
                // При UV-параметризации текстурными координатами будут u и v.
                vertex.texCoord = {1.f - u, v};

                vertices.push_back(vertex);
            }
        }
    }

    void MakeTriangleStripIndicies(unsigned columnCount, unsigned rowCount)
    {
        indicies.clear();
        indicies.reserve((columnCount - 1) * rowCount * 2);
        // вычисляем индексы вершин.
        for (unsigned ci = 0; ci < columnCount - 1; ++ci)
        {
            if (ci % 2 == 0)
            {
                for (unsigned ri = 0; ri < rowCount; ++ri)
                {
                    unsigned index = ci * rowCount + ri;
                    indicies.push_back(index + rowCount);
                    indicies.push_back(index);
                }
            }
            else
            {
                for (unsigned ri = rowCount - 1; ri < rowCount; --ri)
                {
                    unsigned index = ci * rowCount + ri;
                    indicies.push_back(index);
                    indicies.push_back(index + rowCount);
                }
            }
        }
    }

    glm::vec3 GetPositionOnSphere(float u, float v)const
    {
        const float radius = 1.f;
        const float latitude = float(M_PI) * (1.f - v); // 𝝅∙(𝟎.𝟓-𝒗)
        const float longitude = float(2.0 * M_PI) * u; // 𝟐𝝅∙𝒖
        const float latitudeRadius = radius * sinf(latitude);

        return { cosf(longitude) * latitudeRadius,
                 cosf(latitude) * radius,
                 sinf(longitude) * latitudeRadius };
    }
};

struct SCubeFace
{
    uint16_t vertexIndex1;
    uint16_t vertexIndex2;
    uint16_t vertexIndex3;
    uint16_t vertexIndex4;
    uint16_t faceIndex;
};

// Вершины куба служат материалом для формирования треугольников,
// составляющих грани куба.
const glm::vec3 CUBE_VERTICIES[] = {
    {-1, +1, -1},
    {+1, +1, -1},
    {+1, -1, -1},
    {-1, -1, -1},
    {-1, +1, +1},
    {+1, +1, +1},
    {+1, -1, +1},
    {-1, -1, +1},
};

// Грани куба служат для триангуляции куба.
const SCubeFace CUBE_FACES[] = {
    {2, 3, 0, 1, static_cast<uint32_t>(CubeFace::Front)},
    {6, 2, 1, 5, static_cast<uint32_t>(CubeFace::Left)},
    {6, 7, 3, 2, static_cast<uint32_t>(CubeFace::Bottom)},
    {3, 7, 4, 0, static_cast<uint32_t>(CubeFace::Right)},
    {1, 0, 4, 5, static_cast<uint32_t>(CubeFace::Top)},
    {5, 4, 7, 6, static_cast<uint32_t>(CubeFace::Back)},
};

struct CCubeTesselator : SMeshDataP3NT2
{
    void Tesselate(const std::vector<CFloatRect> &textureRects)
    {
        indicies.reserve(6 * 6);
        vertices.reserve(4 * 6);
        for (const SCubeFace &face : CUBE_FACES)
        {
            const CFloatRect texRect = textureRects.at(face.faceIndex);
            const glm::vec3 &coord1 = CUBE_VERTICIES[face.vertexIndex1];
            const glm::vec3 &coord2 = CUBE_VERTICIES[face.vertexIndex2];
            const glm::vec3 &coord3 = CUBE_VERTICIES[face.vertexIndex3];
            const glm::vec3 &coord4 = CUBE_VERTICIES[face.vertexIndex4];
            const glm::vec3 normal = glm::normalize(
                        glm::cross(coord2 - coord1, coord3 - coord1));

            const SVertexP3NT2 v1 = { coord1, texRect.GetTopLeft(), normal};
            const SVertexP3NT2 v2 = { coord2, texRect.GetTopRight(), normal};
            const SVertexP3NT2 v3 = { coord3, texRect.GetBottomRight(), normal};
            const SVertexP3NT2 v4 = { coord4, texRect.GetBottomLeft(), normal};

            const uint32_t fromIndex = uint32_t(vertices.size());
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v4);

            uint32_t newIndicies[] = {
                fromIndex + 0, fromIndex + 1, fromIndex + 2,
                fromIndex + 0, fromIndex + 2, fromIndex + 3,
            };
            std::copy(std::begin(newIndicies), std::end(newIndicies),
                      std::back_inserter(indicies));
        }
    }

    // Если поверхность должна быть видима изнутри, то можно
    //  просто поменять порядок перечисления индексов на обратный.
    void InvertFaces()
    {
        std::reverse(indicies.begin(), indicies.end());
    }
};
}

std::unique_ptr<CMeshP3NT2> CTesselator::TesselateSphere(unsigned precision)
{
    assert(precision >= MIN_PRECISION);

    CSphereTesselator tesselator;
    tesselator.Tesselate(precision, precision);

    auto pMesh = std::make_unique<CMeshP3NT2>(MeshType::TriangleStrip);
    pMesh->Copy(tesselator);
    return pMesh;
}

std::unique_ptr<CMeshP3NT2> CTesselator::TesselateSkybox(const std::vector<CFloatRect> &textureRects)
{
    CCubeTesselator tesselator;
    tesselator.Tesselate(textureRects);
    tesselator.InvertFaces();

    auto pMesh = std::make_unique<CMeshP3NT2>(MeshType::Triangles);
    pMesh->Copy(tesselator);
    return pMesh;
}
