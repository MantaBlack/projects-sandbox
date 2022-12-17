#include <stdio.h>
#include <stdlib.h>

#include <glad\glad.h>  // order is important. GLAD comes first
#include <GLFW\glfw3.h>

#define CHECK_NULL(val, msg) \
    { \
        if (val == NULL) \
        { \
            fprintf_s(stderr, "\n%s\nLocation : %s:%d\n", msg, __FILE__, __LINE__ ); \
            exit(EXIT_FAILURE); \
        } \
    }

enum
{
    WINDOW_HEIGHT     = 600,
    WINDOW_WIDTH      = 800,
    VIEWPORT_LEFT_X   = 0,
    VIEWPORT_BOTTOM_Y = 0
};


static char* read_file_to_string(char* contents, const char* filename, size_t* length)
{
    *length = 0;
    FILE* f = fopen(filename, "rb");
    free(contents);
    contents = NULL;

    if (f)
    {
        fseek(f, 0, SEEK_END); // go to end of file
        // since file is open in binary mode, this is the number of bytes
        // from the beginning of the file
        *length = ftell(f);
        fseek(f, 0, SEEK_SET); // go to beginning of file

        contents = (char*)  malloc(*length + 1);
        CHECK_NULL(contents, "malloc() failed");

        contents[*length] = '\0';

        fread(contents, 1, *length, f);

        fclose(f);

    }

    return contents;
}

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
                                          "Hello Triangle",
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

    /** define a single triangle
     * a vertex is defined in normalized device coordinates (NDC), in [-1, 1]
     */
    float vertices[] = 
    {
        -0.5f, -0.5f, 0.0f, // bottom-left vertex (x, y, z)
         0.5f, -0.5f, 0.0f, // bottom-right
         0.0f,  0.5f, 0.0f  // top
    };

    // create a vertex buffer object (VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    /** VBO is a GL_ARRAY_BUFFER type.
     * we can bind to several buffers at once as long as they have different
     * buffer types.
     * Bind the newly created buffer to the target buffer type
     */
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // copy vertices into the buffer
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW); // data is set once and used many times

    // read source for vertex shader into a string
    char* vertex_shader_source = NULL;
    size_t length = 0;

    vertex_shader_source = read_file_to_string(vertex_shader_source,
                                               "vertex_shader.glsl",
                                               &length);

    /** Compile the vertex shader
     * 1. Create a shader object, identified by a unique ID
     * 2. Attach the shader source code to the shader object
     * 3. Compile shader
     */
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader,
                   1, // how many strings being passed as source code
                   (const GLchar* const *) &vertex_shader_source,
                   NULL);
    glCompileShader(vertex_shader);

    /** checking for compilation errors
     */
    int status = 0;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        fprintf_s(stderr, "glCompileShader() failed:\n%s", info_log);
    }

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