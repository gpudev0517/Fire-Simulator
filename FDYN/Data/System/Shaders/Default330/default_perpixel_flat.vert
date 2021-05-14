#version 330 core

// Computations are done in the eye space
// Those variables are passed to the fragment shader
//flat out vec3 normal;
//out vec3 vertex;
//out vec4 vertexColor;

// Computations are done in the eye space
// Those variables are passed to the fragment shader
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normalIn;
layout (location = 3) in vec4 colorIn;

flat out vec3 normal;
out vec3 vertex;
out vec4 vertexColor;

uniform mat4 modelview;
uniform mat4 projection;

uniform bool useVertexColor;
uniform vec4 meshColor;

void main()
{
	mat3 normalMatrix = (mat3(modelview));

	// Normalize the supplied normal, and transform it
	normal    = normalMatrix * normalIn;
	
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);
	vertex = clipVertex.xyz;
   
	
    // Keep the per-vertex color   
	if( useVertexColor == false )
		vertexColor = meshColor;
	else
		vertexColor = colorIn;
	
    // Make the model-view-projection transform
    gl_Position = projection * clipVertex;
}
