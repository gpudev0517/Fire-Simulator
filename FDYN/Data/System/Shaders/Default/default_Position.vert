#version 330 core

layout(location = 0) in vec3 position;

out vec4 color;
out vec4 modelPos;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelMatrix;

uniform vec4 meshColor;

void main()
{
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;

	modelPos = modelMatrix*vec4(position,1);
	color = meshColor;
}