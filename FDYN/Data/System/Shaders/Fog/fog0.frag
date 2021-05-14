#version 120

// Screen width
uniform float width;
// Screen height
uniform float height;

// The fog's shape arameter
uniform float fogExponent;
// Fluid and rigid view depth
uniform sampler2D tex0;
uniform float pointSizeFactor;

// Position of center in eye space
varying vec3 posEye;
// The point size of a particle
varying float pointSize;

// Main function
void main()
{
	// Discard fragments occluded by rigids
	vec2 screenCoord = gl_FragCoord.xy;
	screenCoord.x /= width;
	screenCoord.y /= height;
	vec2 tex0Vals = texture2D(tex0, screenCoord).xy;
	
	float softParticleFactor = 1.0;
	// Soft Particles for Fluid
	if(tex0Vals.x < posEye.z)
	{
		float dist = (tex0Vals.x - posEye.z) / pointSizeFactor*0.5;
		if(dist > 1.0)
			discard;
		else
			softParticleFactor = exp(dist);
	}	
	// Soft Particles for Mesh
	if(tex0Vals.y < posEye.z)
	{
		float dist = (tex0Vals.y - posEye.z) / pointSizeFactor*0.5;
		if(dist > 1.0)
			discard;
		else
			softParticleFactor = exp(dist);
	}
	

	// Calculate distance from texture coordinates
	vec2 t = (gl_TexCoord[0].xy + vec2(-0.5, -0.5))*2.0;
	float dist = sqrt(t.x*t.x + t.y*t.y);
	// Discard pixels outside circle
    if (dist > 1.0) discard;
	
	float alpha = 1.0 - dist;
	alpha = pow(alpha, fogExponent) * softParticleFactor;
	//alpha = softParticleFactor;
	//alpha = 1.0;
	
	gl_FragData[0] = vec4(0.0, 0.0, 0.0, alpha);
	//gl_FragColor = vec4(1.0, 0.0, 0.0, alpha);
	return;
}