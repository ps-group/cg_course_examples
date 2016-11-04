#pragma once
#include <boost/noncopyable.hpp>
#include <vector>

// Указывает на один из слотов, к которым можно
//  подключать буфер вершинных данных.
enum class BufferType
{
    Attributes, // GL_ARRAY_BUFFER
    Indicies,   // GL_ELEMENT_ARRAY_BUFFER
};

// Константы BufferUsage - это подсказки видеодрайверу,
//   позволяющие выбрать оптимальное местоположение буфера в видеопамяти.
enum class BufferUsage
{
    // Данные обновляются крайне редко.
    StaticDraw,
    // Данные обновляются, но не на каждом кадре.
    DynamicDraw,
    // Данные будут обновляться на каждом кадре.
    StreamDraw,
};

// Класс для работы с Vertex Buffer Object
//  https://www.opengl.org/wiki/Vertex_Specification#Vertex_Buffer_Object
class CBufferObject : private boost::noncopyable
{
public:
    CBufferObject(BufferType type, BufferUsage usageHint = BufferUsage::StaticDraw);
    ~CBufferObject();

    // Отменяет привязку буфера данных заданного типа.
    static void Unbind(BufferType type);

    // Привязывает буфер данных к состоянию контекста OpenGL.
    void Bind() const;

    // Отвязывает буфер данных от состояния контекста OpenGL.
    void Unbind()const;

    // Копирует массив данных в память буфера.
    // Перед копированием происходит привязка.
    void Copy(const void *data, unsigned byteCount);

    // Копирует std::vector с данными в память буфера.
    template <class T>
    void Copy(const std::vector<T> &array)
    {
        const size_t byteCount = sizeof(T) * array.size();
        this->Copy(array.data(), byteCount);
    }

    unsigned GetBufferSize()const;

private:
    unsigned m_bufferId = 0;
    BufferType m_bufferType;
    BufferUsage m_usageHint;
};
