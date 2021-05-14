#version 120

/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader applies accumulated foam intensities via additie blending. 
 * The depth test and depth mask have to be disabled. Further, foam fragments
 * occluded by rigids are discarded and fragments inside the fluid are attenuated.
 * To make Screen Space Ambient Occludion work better, the depth of each fragment is 
 * shifted to created a sphere-like look.
 * 
 */

 // Point size in world space
uniform float pointRadius;  
// Scale to calculate size in pixels
uniform float pointScale;   
// Screen width	
uniform float width;
// Screen height
uniform float height;

// Foam particle type
attribute float type;			
// The left foam particle's lifetime in [0,1]
attribute float lifetime;
// The size modificator for this particle
attribute float sizeMod;

// Position of center in eye space
varying vec3 posEye;        

// The final point sprite's size
varying float pointSize;
varying float fragType;
varying float fragLifetime;
varying float fragSizeMod;

// Main function
void main()
{
    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
    // calculate window-space point size
    posEye = vec3(gl_ClipVertex);
    //float dist = length(posEye.z);
    float dist = abs(posEye.z);
    
	gl_PointSize = pointRadius * (pointScale / dist)*sizeMod;
		
    pointSize = gl_PointSize;
	
	fragType = type;
	fragLifetime = lifetime;
	fragSizeMod = sizeMod;
 
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);

    gl_FrontColor = gl_Color;
}