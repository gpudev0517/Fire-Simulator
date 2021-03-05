#version 120

// Screen width
uniform float width;
// Screen height
uniform float height;

uniform float colorScale;



// Modifies the intensitiy of the fragment
uniform float densityModifier;
// Modifies the densities exponentially
uniform float densityModifierExp;

// Foam intensities
uniform sampler2D tex1;

vec4 hsv_to_rgb(float h, float s, float v, float a)
{
	float c = v * s;
	h = mod((h * 6.0), 6.0);
	float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
	vec4 color;
 
	if (0.0 <= h && h < 1.0) {
		color = vec4(c, x, 0.0, a);
	} else if (1.0 <= h && h < 2.0) {
		color = vec4(x, c, 0.0, a);
	} else if (2.0 <= h && h < 3.0) {
		color = vec4(0.0, c, x, a);
	} else if (3.0 <= h && h < 4.0) {
		color = vec4(0.0, x, c, a);
	} else if (4.0 <= h && h < 5.0) {
		color = vec4(x, 0.0, c, a);
	} else if (5.0 <= h && h < 6.0) {
		color = vec4(c, 0.0, x, a);
	} else {
		color = vec4(0.0, 0.0, 0.0, a);
	}
 
	color.rgb += v - c;
 
	return color;
}


// Main function
void main()
{	
	vec2 screenCoord = vec2(gl_FragCoord.x/width, gl_FragCoord.y/height);
	float intensitySample = texture2D(tex1, screenCoord).w;
	float intensity = intensitySample;

	if(densityModifier + intensitySample > 0.0)
	{
		//intensitySample = 1.0-pow(0.8, intensitySample);
		intensitySample = pow(intensitySample, densityModifierExp);
		intensitySample = intensitySample/(densityModifier + intensitySample);

	}
	
	gl_FragData[0] = vec4(0.0, 0.0, 0.0, intensitySample);

	float redRange = 1;
	float greenRange = 5;
	float blueRange = 10;

	vec4 color = vec4(0,0,0,0);



	if(intensity > 0)
	{
		/*if(intensity > colorScale)
			gl_FragData[1] = vec4(1,1,1,1);

		else*/
		//{
			intensity = clamp(intensity, 0, colorScale);
			gl_FragData[1] = hsv_to_rgb(1.0-clamp((pow(intensity/colorScale, 0.5)+0.3),0, 1), 1, 1, 1);
		//}


	}

	//gl_FragData[1] = color;




	/*float thickness = texture2D(tex1, screenCoord).w;
	float div = 10.0;
	if(thickness > 0.0)
		gl_FragData[1] = vec4(thickness/div, 1-thickness/div, 0.0, 1.0);
	else
		gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);

	return;*/
}
