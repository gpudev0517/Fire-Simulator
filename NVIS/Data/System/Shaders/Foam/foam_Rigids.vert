#version 120

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform bool lighting;
uniform bool picked;
uniform bool ortho;	// if the camera does orthographic projection
uniform float searchRadius;

varying vec3 posEye;        // position of center in eye space
varying float fragSearchRadius;

void main()
{
    // calculate window-space point size
    posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));  
		
    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	
	// Shift the x-coordinate, calculate the projection and save the difference as the search radius
	vec4 vv = vec4(posEye.x + 0.1, posEye.yz, 1.0);
	vec4 vvv = gl_ProjectionMatrix * vv;
	fragSearchRadius = abs(vvv.x/vvv.w - gl_Position.x/gl_Position.w);
	
    gl_FrontColor = gl_Color;
}