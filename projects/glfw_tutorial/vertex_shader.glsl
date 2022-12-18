#version 460 core

/** create an input variable called aPos of type, vec3.
 * set its location to 0
 */
layout (location = 0) in vec3 aPos;

/** we want the vertex shader to determine the color for the fragment
 * shader. For that we need to create an output variable
 */
out vec4 vertex_color;

void main()
{
    /** at the end of main(), whatever value that gl_Position has would the
     * output of the vertex shader.
     */
    gl_Position = vec4(aPos, 1.0f); // we directly put the vec3 in

    // set color to some dark red
    vertex_color = vec4(0.5f, 0.0f, 0.0f, 1.0f);
}