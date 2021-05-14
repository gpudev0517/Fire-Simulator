#version 130
// A basic Sobel edge detection shader.  Detects edges in
// primaryUnit texture, and using a threshold either returns
// a fragment as black (silhouette or crease) or as the color
// stored in colorUnit texture

in vec2 texCoord;


out vec4 FragColor;

uniform sampler2D primaryUnit;
uniform sampler2D colorUnit;

uniform float threshold = 0.25;

void main(void)
{
	const float offset = 1.0 / 1024.0;

	// the texture offset values
	//		tl  t	tr
	//		 l	c	r
	//		bl	b	br
	
	vec4 c  = texture2D( primaryUnit, texCoord);
	vec4 bl = texture2D( primaryUnit, texCoord + vec2( -offset, -offset ) );
	vec4 l  = texture2D( primaryUnit, texCoord + vec2( -offset,     0.0 ) );
	vec4 tl = texture2D( primaryUnit, texCoord + vec2( -offset,  offset ) );
	vec4 t  = texture2D( primaryUnit, texCoord + vec2(    0.0,   offset ) );
	vec4 tr = texture2D( primaryUnit, texCoord + vec2(  offset,  offset ) );
	vec4 r  = texture2D( primaryUnit, texCoord + vec2(  offset,     0.0 ) );
	vec4 br = texture2D( primaryUnit, texCoord + vec2(  offset,  offset ) );
	vec4 b  = texture2D( primaryUnit, texCoord + vec2(    0.0,  -offset ) );
	
	// the Sobel edge detection kernels
	//  x = -1  0  1	y = -1 -2 -1
	//      -2  0  2		 0  0  0
	//      -1  0  1		 1  2  1
	
	vec4 x = -(tl + 2.0 * l + bl) + (tr + 2.0 * r + br);
	vec4 y = -(tl + 2.0 * t + tr) + (bl + 2.0 * b + br);
	
	vec4 pixel = sqrt(x * x + y * y);
	
	if (pixel.x >= threshold || pixel.y >= threshold || pixel.z >= threshold)
	{ 
		vec4 cColor = texture2D(colorUnit, texCoord);
		FragColor = cColor * 0.5;
	}
	else
	{ 
		FragColor = texture2D( colorUnit, texCoord );
	}
	
	//vec4 cc = texture2D( colorUnit, texCoord );
	//FragColor = vec4( cc.w, cc.w, cc.w, 0 );
	//FragColor = texture2D(colorUnit, texCoord);
	//FragColor = vec4( 1, 0, 0, 1);
}