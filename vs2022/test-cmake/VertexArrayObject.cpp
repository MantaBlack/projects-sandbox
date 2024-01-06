#include "VertexArrayObject.hpp"

VertexArrayObject::VertexArrayObject()
{
    // generate buffers with 1 element each
    // order is important. VAO before VBO!!!
    glGenVertexArrays(1, &id);
}

void VertexArrayObject::link_vertex_buffer(VertexBufferObject& vbo, GLuint layout)
{
    vbo.bind();

    // tell OpenGL how to interpret the buffer
    glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(layout);

    vbo.unbind();
}

void VertexArrayObject::bind()
{
    // bind buffers to make them the current references in use
    glBindVertexArray(id);
}

void VertexArrayObject::unbind()
{
    glBindVertexArray(0);
}

void VertexArrayObject::cleanup()
{
    glDeleteVertexArrays(1, &id);
}
