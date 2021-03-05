#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normalIn;
layout(location = 3) in vec4 colorIn;


layout(location = 0) out vec3 N;
layout(location = 1) out vec2 tc0;

uniform mat4 modelview;
uniform mat4 projection;


void main()
{
	mat3 normalMatrix = (mat3(modelview));

	// Normalize the supplied normal, and transform it
	N    = normalMatrix * normalIn;
	
	N = normalize(N);
	vec4 clipVertex = modelview * vec4(position, 1.0);
	
    tc0		  = texCoords;
	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;
}