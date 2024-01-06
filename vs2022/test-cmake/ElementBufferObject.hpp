#pragma once

#include <glad/glad.h>

class ElementBufferObject
{
public:
    GLuint id;

    ElementBufferObject(GLuint* indices, GLsizeiptr size);

    void bind();
    void unbind();
    void cleanup();
};