#version 330 compatibility 

layout (location = 0) in vec3 inPos;

out vec3 worldPos;

void main()
{
    vec4 pos = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(inPos, 1.0);
    gl_Position = pos;
    worldPos = inPos;
}
