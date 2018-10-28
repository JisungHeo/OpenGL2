#version 440 core
// myVS.glsl
// Pass-through vertex shader to do the fixed functionality.
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;

uniform mat4 MVP;
out vec3 fragmentColor;
void main()
{
    // Method #1)
    //gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
 
    // Method #2)
    gl_Position = MVP * vec4(vertexPosition, 1);
	fragmentColor = vertexColor;
 
    // Method #3)
    //gl_Position = ftransform();
}
 
// gl_Position: transformed vertex (output)
// gl_ProjectionMatrix (projection matrix)
// gl_ModelViewMatrix (modelview matrix)
// gl_Vertex: vertex in model coordinates (input)
// Use Method #3 to get the invariance with fixed functionality.