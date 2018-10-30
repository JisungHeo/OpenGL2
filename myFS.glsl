#version 440 core
// myFS.glsl
in vec3 fragmentColor;
out vec3 color;
void main()
{
    color = fragmentColor;
}
 
// gl_FragColor: resulting fragment color (output)