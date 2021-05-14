#version 130

in vec3 position;
in vec2 texCoord;

out vec2 texc;

uniform mat4 projection;
uniform mat4 view;
uniform float envScale;
uniform float envRotation;

mat3 getEnvRotationMatrix()
{
    return mat3(
        cos(envRotation), 0, sin(envRotation),
        0, 1, 0,
        -sin(envRotation), 0, cos(envRotation)
    );
}

void main()
{
    vec4 pos = projection * view * vec4(getEnvRotationMatrix() * position*envScale*100, 1.0);
    gl_Position = pos;
    texc = texCoord;
}