#version 330

// Computations are done in the eye space
// Those variables are passed to the fragment shader
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normalIn;
layout(location = 3) in vec4 colorIn;

out vec2 tc0;
out vec4 worldPos;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 modelMatrix;


void main()
{

	// Normalize the supplied normal, and transform it
	tc0		  = texCoords;
	tc0.x     = texCoords.y;
	tc0.y     = 1 - texCoords.x;
	
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);
	
	worldPos = modelMatrix*vec4(position,1);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;
}
