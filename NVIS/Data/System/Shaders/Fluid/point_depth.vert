#version 120

#if (__VERSION__ > 210)
layout(location = 0) in vec4 Color;
layout(location = 1) in vec3 Position;
out vec3 fs_Position;

uniform mat4 modelview;
uniform mat4 projection;
#endif

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels


void main()
{
#if (__VERSION__ > 210)
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(Position, 1.0);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;


    // calculate window-space point size
	gl_PointSize = pointScale * (pointRadius / gl_Position.w);

	fs_Position = clipVertex.xyz;
#else
	// calculate window-space point size
	gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	gl_PointSize = pointScale * (pointRadius / gl_Position.w);

	gl_TexCoord[0] = gl_MultiTexCoord0;    
	gl_TexCoord[1] = gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0);
#endif
}