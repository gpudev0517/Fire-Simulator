#version 330 core

#if (__VERSION__ > 210)
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 inormal;
layout(location = 3) in vec4 colorIn;

out vec4 color;
out vec4 modelPos;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelMatrix;
uniform vec4 meshColor;
uniform bool useVertexColor;

void main()
{
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;

	modelPos = modelMatrix*vec4(position,1);
	if (useVertexColor == true)
		color = colorIn;
	else
		color = meshColor;
}
#else
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	gl_TexCoord[0] = gl_Color;
}
#endif