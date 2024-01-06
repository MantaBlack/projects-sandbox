#pragma once

#include <glad/glad.h>

class VertexBufferObject
{
public:
    GLuint id;

    VertexBufferObject(GLfloat* vertices, GLsizeiptr size);

    void bind();
    void unbind();
    void cleanup();
};