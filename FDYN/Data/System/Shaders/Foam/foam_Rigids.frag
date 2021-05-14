#version 120

// Point size in world space
uniform float pointRadius; 
uniform float nearClip;
uniform float farClip;
uniform bool lighting;
uniform bool picked;

// Position of center in eye space
varying vec3 posEye;        
// The search radius of the filter given in pixels
varying float fragSearchRadius;

void main()
{	
	gl_FragData[0] = vec4(gl_FragCoord.z, -posEye.z, fragSearchRadius, 0.0);
	return;
}
