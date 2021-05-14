#version 120

/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader filters shadow values produced by SSAO. 
 * A gauﬂian filter is applied to achieve this.
 * In the end, the shadows and intensities are merged with a pre-rendered
 * backround image.
 * 
 */

// Screen width
uniform float width;
// Screen height
uniform float height;

uniform int sourceChannel;

uniform vec3 destWeights;

// Texture
uniform sampler2D tex0;


// Main function
void main()
{
	vec2 screenCoord = gl_FragCoord.xy;
	screenCoord.x /= width;
	screenCoord.y /= height;
	gl_FragColor = vec4(destWeights*texture2D(tex0, screenCoord)[sourceChannel], 1.0);
	return;
}
