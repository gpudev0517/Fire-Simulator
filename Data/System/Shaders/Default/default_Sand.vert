#version 330 core

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform bool lighting;
uniform bool picked;
uniform bool ortho;	        // if the camera does orthographic projection

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec2 normalIn;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec3 vertex;
layout (location = 2) out vec4 vertexColor;
layout (location = 4) out vec3 posEye;
layout (location = 5) out vec3 normalEye;
layout (location = 6) out float pointSize;



//Varying variables are set by the vertex shader and passed to the fragment shader
//varying vec3 posEye;        // position of center in eye space
//varying vec3 normalEye;     //normal vector of point in eye space
//varying vec3 lightDir;
//varying float pointSize;
//varying vec4 vertexColor;
//varying vec3 vertex;

/*
A Point-Sprites is based and defined by one single point. 
The GPU automatically generates a quad around this point. The edge-size of the quad is defined by gl_PointSize.
The quad always faces the scene's beholder, so it behaves like a billboard or 2D sprite. 
The GPU also creates a texture-coordinate for each of the quad's corners (0,0) (0,1) (1,0) and (1,1).
*/

void main()
{
    //transform vertex to eye space
    posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));
    //distance to camera is equal to abs(posEye.z)
    float dist = abs(posEye.z);
    
    //The normal matrix is the transpose of the inverse of the modelview matrix,
    // see http://www.lighthouse3d.com/opengl/glsl/index.php/index.php?normalmatrix
    normalEye = normalize(gl_NormalMatrix * gl_Normal);
    //lightDir = normalize(vec3(3,1,1));
    
    //calculate point size, glPointSize is size of particle in screen units (=pixels)
    //based on viewport width and distance to camera
    if(!ortho)
		gl_PointSize = pointRadius * (pointScale / dist);
	else
		gl_PointSize = pointRadius * pointScale;
		
	pointSize = gl_PointSize;
	
	//for clipping project vertex in eye space
    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
    
	//gl_TexCoord is a varying variable, used in the fragment shader to access the interpolated texture coordinate
    gl_TexCoord[0] = gl_MultiTexCoord0;
	
	//here gl_Color is the value set by glColor, either provided by the vertex or in glCode
	//the fragment shade can access gl_FrontColor using gl_Color
    gl_FrontColor = gl_Color;
    vertexColor = gl_Color;
    
    //transform position, gl_Position is the final position of the vertex
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	
	vertex = gl_Position.xyz;
}