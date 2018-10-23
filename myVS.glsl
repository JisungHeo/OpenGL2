// myVS.glsl
// Pass-through vertex shader to do the fixed functionality.
void main()
{
    // Method #1)
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
 
    // Method #2)
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 
    // Method #3)
    //gl_Position = ftransform();
}
 
// gl_Position: transformed vertex (output)
// gl_ProjectionMatrix (projection matrix)
// gl_ModelViewMatrix (modelview matrix)
// gl_Vertex: vertex in model coordinates (input)
// Use Method #3 to get the invariance with fixed functionality.