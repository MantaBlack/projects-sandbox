#version 460 core

// define an output variable for the fragment shader
out vec4 frag_color;

/** this is the output from the vertex shader. We need to make sure it is
 * the same name and data type. OpenGL will link them automatically
 */
in vec3 vertex_color;

/** a Uniform is global, i.e., it is unique per shader program object.
 * It is used to send data from the CPU to shaders on the GPU. It can be
 * accessed by any shader, at any stage, in a given shader program. They will
 * keep their values until reset or updated.
 * 
 * !!! If you declare a uniform that isn't used anywhere in your GLSL code the
 * compiler will silently remove the variable from the compiled version which
 * is the cause for several frustrating errors; keep this in mind !!!
 * 
 * It can be defined in any shader. Since we're not using it in the vertex
 * shader, we don't define it there
 * 
 * Here, let's use it to set the color from the CPU
 */
// uniform vec4 our_color;

void main()
{
    // frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    
    // frag_color = vertex_color; //setting color from vertex shader

    // frag_color = our_color; // use with Uniform

    frag_color = vec4(vertex_color, 1.0f);
}