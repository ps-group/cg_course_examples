#pragma once

// Класс для работы с Vertex Array Object
//  https://www.opengl.org/wiki/Vertex_Specification#Vertex_Array_Object
// Создание хотя бы одного VAO обязательно в OpenGL 3.0+ Core Profile.
class CArrayObject
{
public:
    struct do_bind_tag {};

    CArrayObject();
    CArrayObject(do_bind_tag);
    ~CArrayObject();

    void Bind();

private:
    unsigned m_arrayId = 0;
};
