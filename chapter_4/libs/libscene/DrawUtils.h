#pragma once
#include "Geometry.h"

class CDrawUtils
{
public:
    CDrawUtils() = delete;

    // Метод для вычисления матрицы, корректно преобразующей нормаль
    //  без искажения из-за неоднородного масштабирования.
    // Неоднородное масштабирование могло бы совершить с нормалями следующее:
    // |          \  /
    // |____  =>   \/
    // |            \
    // |             \
    // http://stackoverflow.com/questions/23210843/
    static glm::mat4 GetNormalMatrix(const glm::mat4 &modelView);

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
