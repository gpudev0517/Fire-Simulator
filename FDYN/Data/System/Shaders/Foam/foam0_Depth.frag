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
uniform float nearClip;
uniform float farClip;

// Position of center in eye space
varying vec3 posEye;        
varying float pointSize;
varying float fragType;
//varying float fragSizeMod;
varying float fragSearchRadius;

// Calculates the projected depth value from an eye space depth
float projectDepth(float z_e)
{
	return (-(farClip + nearClip)/(farClip - nearClip)*z_e - (2.0*farClip*nearClip)/(farClip - nearClip))/(-z_e);
}

// Main function
void main()
{
	// Calculate distance from texture coordinates
	vec3 n;
	n.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
	float dist2 = dot(n.xy, n.xy);
	// Discard pixels outside circle

	//gl_FragDepth = 1.0;

   

    //float dist = sqrt(dist2);

	// Calculate sphere depth by texture coordinates
	n.z = sqrt(1.0 - n.x*n.x - n.y*n.y);

	// Shift the eye space depth by the sphere depth and project it
	float sphereDepth = (projectDepth(posEye.z + pointRadius*n.z) + 1.0)/2.0;
	// Assign the new depth value

	gl_FragDepth = sphereDepth;

	 if (dist2 > 1.0) discard;  
	
	// Fragment depth, eye space depth, and fragment search radius are written to the texture's r,g,b channels
	gl_FragData[0] = vec4(0.0, 0.0, -(posEye.z + pointRadius*n.z), fragSearchRadius);

	mat3 invNormal = transpose(gl_NormalMatrix);
	gl_FragData[1] = vec4(invNormal*n, fragType);

	vec4 eyePos = vec4(posEye.x, posEye.y, posEye.z + pointRadius*n.z, 1.0);
	/*vec4 worldPos = gl_ModelViewMatrixInverse*eyePos;
	gl_FragData[2] = worldPos;
*/
	//gl_FragData[2] = vec4(n, 0.0);

	return;
}
