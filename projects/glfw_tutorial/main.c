#include <stdio.h>

#include <glad\glad.h>  // order is important. GLAD comes first
#include <GLFW\glfw3.h>

enum
{
    WINDOW_HEIGHT     = 600,
    WINDOW_WIDTH      = 800,
    VIEWPORT_LEFT_X   = 0,
    VIEWPORT_BOTTOM_Y = 0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(int argc, char const *argv[])
{
    // initialize GLFW
    glfwInit();

    // configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create a window object
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH,
                                          WINDOW_HEIGHT,
                                          "LearnOpenGL",
                                          NULL,
                                          NULL);

    if (window == NULL)
    {
        fprintf_s(stdout, "Failed to create GLFW window\n");
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    // initialize GLAD before calling any OpenGL function
    /**
     * We pass GLAD the function to load the address of the OpenGL function
     * pointers which is OS-specific.
     */
    if ( !gladLoadGLLoader((GLADloadproc) glfwGetProcAddress) )
    {
        fprintf_s(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    /**
     * Tell OpenGL the size of the rendering window so that it knows how we
     * want to display the data and coordinates with respect to the window.
     * OpenGL coordinates are between -1 and 1.
     * The window size here could be smaller than the one specified in GLFW.
     */
    glViewport(VIEWPORT_LEFT_X,
               VIEWPORT_BOTTOM_Y,
               WINDOW_WIDTH,
               WINDOW_HEIGHT);

    // register a callback that handles window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /**
     * This is the render loop.
     * An iteration is usually called a frame.
     * glfwWindowShouldClose checks at the start of every loop if GLFW has been
     * instructed to close.
     */
    while (!glfwWindowShouldClose(window))
    {
        /** clear only the color buffer at the start of a frame with a color
         * of our choice
         */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // this will values set by glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

        /** Handle user inputs
         */
        process_input(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up after render loop is done
    glfwTerminate();

    return 0;
}