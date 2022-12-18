#version 460 core

/** this vertex shader receives both position and color values
 */

layout (location = 0) in vec3 position_value;
layout (location = 1) in vec3 color_value;

/** we want the vertex shader to determine the color for the fragment
 * shader. For that we need to create an output variable
 */
out vec3 vertex_color;

void main()
{
    gl_Position  = vec4(position_value, 1.0);
    vertex_color = color_value;
}