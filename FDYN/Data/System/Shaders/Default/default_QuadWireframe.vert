#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 barycentric;

out vec3 vBarycentric;
out vec4 vModelPos;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelMatrix;


void main()
{
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);
	
	vBarycentric = barycentric;
	vModelPos = modelMatrix*vec4(position,1);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;

}