#version 120

/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader creates the depth field of the foam. The each particle is rendered as a point sprite.
 * The fragments' depth values are transpormed give the point spite a sphere-like appearance.
 * Further, a search radius for Screen Space Ambient Occlusion is calculated by shifting the eye space positions
 * and projection the difference.
 * 
 */

// Point size in world space
uniform float pointRadius;  
// Scale to calculate size in pixels
uniform float pointScale;   

uniform float oneOverTanFovOver2;
// The size modificator for this particle
attribute float sizeMod;

attribute float type;

// Position of center in eye space
varying vec3 posEye;
//varying vec3 posWorld;        
varying float pointSize;
varying float fragSearchRadius;
varying float fragType;
//varying float fragSizeMod;

// Main function
void main()
{
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
    // Calculate window-space point size
    posEye = vec3(gl_ClipVertex);
    //posWorld = vec3(gl_Vertex);
    float invDist = 1.0/abs(posEye.z);
    
	gl_PointSize = pointRadius * (pointScale * invDist)*sizeMod;
		
    pointSize = gl_PointSize;
	
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	
	// Shift the x-coordinate
	//vec4 vv = vec4(posEye.x + pointRadius*sizeMod, posEye.yz, 1.0);
	// Project the shifted position
	//vec4 vvv = gl_ProjectionMatrix * vv;
	// Save the difference as the search radius
	//fragSearchRadius = abs(vvv.x/vvv.w - gl_Position.x/gl_Position.w);

	fragSearchRadius = pointRadius * (oneOverTanFovOver2* invDist)*sizeMod;

	fragType = type;
	
	//fragSizeMod = sizeMod;

    gl_FrontColor = gl_Color;
}