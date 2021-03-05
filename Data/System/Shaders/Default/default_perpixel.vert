#version 410 core

// Computations are done in the eye space
// Those variables are passed to the fragment shader
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normalIn;
layout(location = 3) in vec4 colorIn;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec3 vertex;
layout(location = 2) out vec4 vertexColor;
layout(location = 3) out vec2 tc0;
layout(location = 4) out vec4 worldPos;
layout(location = 5) out vec3 localPos;

uniform mat4 modelview;
uniform mat4 projection;

uniform bool useVertexColor;
uniform vec4 meshColor;
uniform mat4 modelMatrix;

void main()
{
	mat3 normalMatrix = mat3(transpose(inverse(modelview)));

	// Normalize the supplied normal, and transform it
	normal    = normalMatrix * normalIn;
	tc0		  = texCoords;
	tc0.x     = texCoords.y;
	tc0.y     = 1 - texCoords.x;
	
	// 
	// for reflection on a spherical map
	//
	vec4 p = vec4( position, 1. );
	vec3 e = normalize( vec3( modelview * p ) );
	vec3 n = normalize( normal );
	
	vec3 r = reflect( e, n );
	float m = 2. * sqrt( pow( r.x, 2. ) + pow( r.y, 2. ) + pow( r.z + 1., 2. ) );

	// For other 
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);
	vertex = clipVertex.xyz;

	if( useVertexColor == false )
		vertexColor = meshColor;
	else
		vertexColor = colorIn;

	localPos = position;
	worldPos = modelMatrix*vec4(position,1);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;
}
