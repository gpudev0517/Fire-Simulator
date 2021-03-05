#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 colorIn;

layout (location = 0) out vec4 Albedo;


uniform mat4 modelview;
uniform mat4 projection;
uniform vec4 meshColor;

void main()
{
    vec4 clipVertex = modelview * vec4( position, 1.0f );

	gl_Position = projection * clipVertex;
	Albedo = meshColor;
}