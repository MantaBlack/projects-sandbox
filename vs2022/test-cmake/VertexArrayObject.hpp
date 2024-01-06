#pragma once

#include <glad/glad.h>
#include "VertexBufferObject.hpp"

class VertexArrayObject
{
public:
    GLuint id;

    VertexArrayObject();

    void link_vertex_buffer(VertexBufferObject& vbo, GLuint layout);

    void bind();
    void unbind();
    void cleanup();
};