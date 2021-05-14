#version 120

// Main function
void main()
{
	// Calculate distance from texture coordinates
	vec2 t = (gl_TexCoord[0].xy + vec2(-0.5, -0.5))*2.0;
	float dist = sqrt(t.x*t.x + t.y*t.y);
	// Discard pixels outside circle
    if (dist > 1.0) discard;
	
	float alpha = 1.0 - dist;
	alpha *= 0.02;
	
	gl_FragData[0] = vec4(1.0, 1.0, 1.0, alpha);
	return;
}