#version 330 core

layout(location = 0) in vec3 position;


uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelMatrix;

out VS_GS_INTERFACE
{
	vec3 vertex;
	vec3 clipVert;
	vec4 modelPos;
} vs_out;

void main()
{
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);
	vs_out.clipVert = clipVertex.xyz;
	vs_out.vertex = vs_out.clipVert;
	vs_out.modelPos = modelMatrix*vec4(position,1);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;

}