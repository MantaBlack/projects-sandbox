#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
    //initialize GLFW
    glfwInit();

    //inform GLFW about OpenGL version we are using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 800;
    const int BOTTOM_LEFT_X = 0;
    const int BOTTOM_LEFT_Y = 0;

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

    // specify a default background color
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    // clear the back buffer and apply the default background color
    glClear(GL_COLOR_BUFFER_BIT);

    // swap the back buffer with the front buffer
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        // make the window respond to events
        glfwPollEvents();
    }

    // clean up window
    glfwDestroyWindow(window);

    //terminate GLFW
    glfwTerminate();

	return 0;
}
