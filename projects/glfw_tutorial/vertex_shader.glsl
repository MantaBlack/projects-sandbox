#version 460 core

/** create an input variable called aPos of type, vec3.
 * set its location to 0
 */
layout (location = 0) in vec3 aPos;

void main()
{
    /** at the end of main(), whatever value that gl_Position has would the
     * output of the vertex shader.
     */
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}