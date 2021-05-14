#version 120

uniform float fogAbsorption;

// Screen width
uniform float width;
// Screen height
uniform float height;
// The fog densities
uniform sampler2D tex0;

// Main function
void main()
{
	// Get screen coordinates
	float invWidth = 1.0/width;
	float invHeight = 1.0/height;
	vec2 screenCoord = vec2(gl_FragCoord.x*invWidth, gl_FragCoord.y*invHeight);
	
	vec4 tex0Val = texture2D(tex0, screenCoord);
	float result = tex0Val.w;
	if(result > 0.0)
		result = 1.0 - pow(fogAbsorption, result);
	//gl_FragData[0] = vec4(result, result, result, result);
	gl_FragData[0] = vec4(1.0, 1.0, 1.0, result);
	return;
}