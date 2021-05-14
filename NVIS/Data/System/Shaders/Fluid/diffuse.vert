#version 120

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform mat4 lightTransform;
uniform float spotMin;
uniform float spotMax;
uniform vec4 color;


void main()
{
	vec3 worldPos = gl_Vertex.xyz;// - vec3(0.0, 0.1*0.25, 0.0);	// hack move towards ground to account for anisotropy;
	vec4 eyePos = gl_ModelViewMatrix * vec4(worldPos, 1.0);

	gl_Position = gl_ProjectionMatrix * eyePos;
	//gl_Position.z -= 0.0025;	// bias above fluid surface

    // calculate window-space point size
	gl_PointSize = pointRadius * (pointScale / gl_Position.w);

	gl_TexCoord[0] = gl_MultiTexCoord0;    
	gl_TexCoord[1] = vec4(worldPos, gl_Vertex.w);
	gl_TexCoord[2] = eyePos;

	gl_TexCoord[3].xyz = (gl_ModelViewMatrix*vec4(gl_MultiTexCoord1.xyz, 0.0)).xyz;
	gl_TexCoord[4].xyzw = color;

	// hack to color different emitters 
	if (gl_MultiTexCoord1.w == 2.0)
		gl_TexCoord[4].xyzw = vec4(0.85, 0.65, 0.65, color.w);
	else if (gl_MultiTexCoord1.w == 1.0)
		gl_TexCoord[4].xyzw = vec4(0.65, 0.85, 0.65, color.w);

	// compute ndc pos for frustrum culling in GS
	vec4 ndcPos = gl_ModelViewProjectionMatrix * vec4(worldPos.xyz, 1.0);
	gl_TexCoord[5] = ndcPos / ndcPos.w;
}