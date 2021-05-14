#version 410 core

#if (__VERSION__ > 210)
// Color-Position shader in OpenGL v4.
layout(location = 0) in vec4 colorIn;
layout(location = 1) in vec3 position;

layout(location = 0) out vec4 color;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;
	
	color = colorIn;
}
#else
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	gl_TexCoord[0] = gl_Color;
}
#endif