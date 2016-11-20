#pragma once
#include "Geometry.h"

class CDrawUtils
{
public:
    CDrawUtils() = delete;

    // Метод для вычисления матрицы, корректно преобразующей нормаль
    //  без искажения из-за неоднородного масштабирования.
    // http://stackoverflow.com/questions/23210843/
    static glm::mat4 GetNormalMat4(const glm::mat4 &modelView);

    // Модифицирует матрицу преобразования в координаты наблюдателя сцены,
    //  чтобы объект оказался размещённым вокруг камеры,
    //  как часть окружения.
    static glm::mat4 GetEnvironmentViewMat4(const glm::mat4 &view);

    // Применяет трансформацию, заданную в `mat`, к точке point.
    static glm::vec3 TransformPoint(const glm::vec3 &point, const glm::mat4 &mat);

    // Выполняет glDrawRangeElements для заданного размещения
    //  вершин и индексов в памяти. Перед вызовом следует:
    // - привязать буферы геометрических данных
    // - активировать шейдерную программу
    // - настроить uniform-переменные
    // - настроить привязку атрибутов вершин к параметрам вершинного шейдера.
    static void DrawRangeElements(const SGeometryLayout &layout);

    // Выполняет привязку активной текстуры, если указатель не нулевой,
    //  иначе привязывает нулевую текстуру (id == 0).
    static void MaybeBind(const CTexture2DSharedPtr &pTexture);
};
