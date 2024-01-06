#include "VertexBufferObject.hpp"

VertexBufferObject::VertexBufferObject(GLfloat* vertices, GLsizeiptr size)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBufferObject::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBufferObject::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBufferObject::cleanup()
{
    glDeleteBuffers(1, &id);
}
