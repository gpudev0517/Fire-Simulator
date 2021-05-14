#version 120

uniform float pointSizeFactor;
// Scale to calculate size in pixels
uniform float pointScale;  

// Position of center in eye space
varying vec3 posEye;  
// The point size of a particle
varying float pointSize;

// Main function
void main()
{
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	
	posEye = -vec3(gl_ClipVertex);
	float invDist = 1.0/abs(posEye.z);
	gl_PointSize = pointSizeFactor*invDist*pointScale;
	pointSize = gl_PointSize;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	gl_FrontColor = gl_Color;
	return;
}