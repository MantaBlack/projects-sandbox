#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderClass.hpp"
#include "ElementBufferObject.hpp"
#include "VertexArrayObject.hpp"
#include "VertexBufferObject.hpp"


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int BOTTOM_LEFT_X = 0;
const int BOTTOM_LEFT_Y = 0;

// define vertices for an equilateral triangle
// viewport is normalized in [-1, 1] so (0,0) in the center
GLfloat vertices[] =
{
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,    // Lower left corner
     0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,    // Lower right corner
     0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, // Upper corner
    -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // Inner left
     0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // Inner right
     0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f     // Inner down
};

// use indices to specify the order in which vertices should be used
GLuint indices[] =
{
    0, 3, 5, // lower left triangle
    3, 2, 4, // upper triangle
    5, 4, 1  // lower right triangle
};


int main()
{
    // initialize GLFW
    glfwInit();

    // inform GLFW about OpenGL version we are using (OpenGL 3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // tell GLFW that we are using the CORE profile (modern functions only)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Window",
        NULL,
        NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // make the new window as part of the current context.
    // make window available for use.
    glfwMakeContextCurrent(window);

    // load current config for OpenGL
    gladLoadGL();

    // tell OpenGL the area of the window to render in
    glViewport(BOTTOM_LEFT_X,
        BOTTOM_LEFT_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);

    Shader shader_program("D:\\PROJECTS\\VS2022\\GIT\\projects-sandbox\\vs2022\\test-cmake\\vertex_shader.glsl",
        "D:\\PROJECTS\\VS2022\\GIT\\projects-sandbox\\vs2022\\test-cmake\\fragment_shader.glsl");
    
    VertexArrayObject vao;
    vao.bind();

    VertexBufferObject vbo(vertices, sizeof(vertices));
    
    ElementBufferObject ebo(indices, sizeof(indices));

    vao.link_vertex_buffer(vbo, 0);
    vao.unbind();
    vbo.unbind();
    ebo.unbind();

    while (!glfwWindowShouldClose(window))
    {
        // specify a default background color
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

        // clear the back buffer and apply the default background color
        glClear(GL_COLOR_BUFFER_BIT);

        // specify which shader program to use
        shader_program.activate();

        vao.bind();

        // this function is used when drawing elements with EBO
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // swap the back buffer with the front buffer
        glfwSwapBuffers(window);

        // make the window respond to events
        glfwPollEvents();
    }

    // clean up
    vao.cleanup();
    vbo.cleanup();
    ebo.cleanup();
    shader_program.cleanup();

    // clean up window
    glfwDestroyWindow(window);

    // terminate GLFW
    glfwTerminate();

    return 0;
}
