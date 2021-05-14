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

// Position of center in eye space
varying vec3 posEye;        
varying float pointSize;
varying float fragType;
varying float fragLifetime;
varying float fragSizeMod;

uniform float nearClip;
uniform float farClip;
// Screen width
uniform float width;
// Screen height
uniform float height;
// The fading depth. If a fragment's depth value is smaler than the fluids depth it 
// is attenuated exponentially using this value.
uniform float falloffDistance;
uniform float falloffExponent;


// Fluid and rigid view depth
uniform sampler2D tex0;


// Calculates the eye space depth value from a depth buffer value
float eyeDepth(float z_n)
{
	return 2.0*farClip*nearClip/(z_n*farClip - farClip - nearClip - nearClip*z_n);
}

// Calculates the projected depth value from an eye space depth
float projectDepth(float z_e)
{
	return (-(farClip + nearClip)/((farClip - nearClip))*z_e - 2.0*farClip*nearClip/(farClip - nearClip))/(-z_e);
}

float fallOff(float d, float s, float n)
{
	if(n < 1.0)
		return 1.0;
	if(d < s)
		return pow((1.0-(d/s)), n);
	else
		return 0.0;
}

float fallOff2(float d, float s, float e1, float e2)
{
	if(abs(d) < abs(s))
		return pow(1.0-pow((d/s), e1), e2);
	else
		return 0.0;
}



// Main function
void main()
{
	// Calculate alpha from texture coordinates
	vec2 t = (gl_TexCoord[0].xy + vec2(-0.5, -0.5))*2.0;
	float alpha2 = t.x*t.x + t.y*t.y;
	// Discard pixels outside the circle
    if (alpha2 > 1.0) discard;

    float alpha = sqrt(alpha2);
	
	// Calculate sphere height by texture coordinates
	float z = sqrt(1.0 - t.x*t.x - t.y*t.y);
	// Shift the eye space depth by the sphere height
	float sphereDepthEye = -(posEye.z + pointRadius*z);
	// Calculate the depth buffer value
	float sphereDepth = (projectDepth(-sphereDepthEye) + 1.0)/2.0;
	// Assign the shifted depth value
	gl_FragDepth = sphereDepth;
	
	// Discard fragments occluded by rigids
	vec2 screenCoord = gl_FragCoord.xy;
	screenCoord.x /= width;
	screenCoord.y /= height;

	vec2 tex0Vals = texture2D(tex0, screenCoord).xy;

	float rigidDepthEye = tex0Vals.y;
	if(rigidDepthEye > 0.0 && rigidDepthEye < sphereDepthEye) discard;
	
	// Apply intensity function depending on the particle type
	// Bubbles
	if(fragType < 1.5/16.0)
		alpha = (pow(alpha, 2.0))*1.0;
	// Foam
	else if(fragType < 2.5/16.0)
		alpha = (1.0 - pow(alpha, 2.25));
	// Spray
	else if(fragType > 5.0/16.0)
		alpha = (1.0 - pow(alpha, 1.5));
	else 
		alpha = 0.0;
	
	// Fade foam that's inside of the fluid
	float sceneDepthEye = tex0Vals.x;

	if(sceneDepthEye > 0.0 && sceneDepthEye < sphereDepthEye)
	{
		//float fade = pow(fallOffDistance, (sphereDepthEye - sceneDepthEye));
		float fade = fallOff(sphereDepthEye - sceneDepthEye, falloffDistance, falloffExponent);
		alpha *= fade;
		if(alpha == 0.0) discard;
	}
	
	// Fade foam by the particle's left lifetime
	alpha *= fallOff2(1.0-fragLifetime, 1.0, 2.0, 0.4);

	gl_FragData[0] = vec4(0.0, 0.0, 0.0, alpha);
}
