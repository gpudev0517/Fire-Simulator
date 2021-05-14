#version 330 core

uniform float pointRadius;  // point size in world space
uniform bool lighting;
uniform bool picked;

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 vertexColor;
layout (location = 4) in vec3 posEye;
layout (location = 5) in vec3 normalEye;
layout (location = 6) in float pointSize;

//Varying variables are passed by vertex shader
//varying vec3 posEye;        // position of center in eye space
//varying float pointSize;
//varying vec3 normalEye; //normal vector of point in eye space


vec4 calculateLighting(in vec3 N, in vec3 PointSpritesNormal); 

/*
The fragment shader gets the interpolated pixels for the quad computed by the vertex shader.
It colorizes and discards pixels such that the quad appears like a 3d sphere (=pseudo sphere).
The texture quad has coordinates (x,y) ranging from (0,0) to (1,1). 
For computing circle and pseudo normal, (x,y) is transformed to (x',y') such that the center (0.5,0.5) is at the origin.
If the length of (x',y') is larger than 1, the pixel is not inside the circle or pseudo sphere and can be discarded.
(0,0)---------					(-1,1)---------
	|		 |						|		  |
	|		 |		----\			|		  |
	|		 |		----/			|		  |
	 --------(1,1)					 --------(1,-1)
	 
- pointSize is the size of the particle in pixels.
*/
void main()
{
	//For all other pixels shade quad as a 3d like sphere
  
	//Apply constant light source	
	//const vec3 lightDir = vec3(0.301, 0.301,  0.904);
	
	//n will represent the pseudo normal of the fragment in camera space
	vec3 n;
    //Compute distance of pixel/fragment to particle position = center of quad/texture    
    //First: transform texture coordinates (x,y) such that (0.5,0.5) is the center, i.e. (0.0)
    n.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);    
    //Lenght of (n.x,n.y) is the distance to the center of quad 
    float mag = dot(n.xy, n.xy);
    //Due to the transformation above all distances larger than 1 are outside of the circle
    if (mag > 1.0) discard;   // kill pixels outside circle

    //If lighting is disabled only a 2d circle will be rendered as sprite
    if(!lighting)
	{
		gl_FragColor = vertexColor;
		return;
	}
	//draw green circle around picked particle, no other shading
	if(picked && mag > 0.5)
	{
		gl_FragColor = vec4(0,1,0,1);
		return;
	}
    
    //Begin of 3d like shading
    //Idea: every pixel will be shaded as point on sphere
    //Pixel at particle center gets normal (0,0,1) -> pointing to camera
    //Pixel with texture coordinates (0,0.5) has n.xy=(0,1) and gets normal (0,1,0)
    n.z = sqrt(1.0-mag);
    n = normalize(n);
	vec3 norm = normalize(normalEye);

    gl_FragColor = calculateLighting(norm, n); 
}