#version 150
#extension GL_EXT_geometry_shader4 : enable

uniform float pointScale;  // point size in world space
uniform float motionBlurScale;
uniform float diffusion;
uniform vec3 lightDir;

uniform mat4 projection;
uniform mat4 modelview;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

out vec4 texCoord[6];

void main()
{
	vec4 ndcPos = gl_TexCoordIn[0][5];

	// frustrum culling
	const float ndcBound = 1.0;
	if (ndcPos.x < -ndcBound) return;
	if (ndcPos.x > ndcBound) return;
	if (ndcPos.y < -ndcBound) return;
	if (ndcPos.y > ndcBound) return;

	float velocityScale = 1.0;

	vec3 v = gl_TexCoordIn[0][3].xyz*velocityScale;
	vec3 p = gl_TexCoordIn[0][2].xyz;
		
	// billboard in eye space
	vec3 u = vec3(0.0, pointScale, 0.0);
	vec3 l = vec3(pointScale, 0.0, 0.0);
	
	// increase size based on life
	float lifeFade = mix(1.0f+diffusion, 1.0, min(1.0, gl_TexCoordIn[0][1].w*0.25f));
	u *= lifeFade;
	l *= lifeFade;

	//lifeFade = 1.0;

	float fade = 1.0/(lifeFade*lifeFade);
	float vlen = length(v)*motionBlurScale;

	if (vlen > 0.5)
	{
		float len = max(pointScale, vlen*0.016);
		fade = min(1.0, 2.0/(len/pointScale));

		u = normalize(v)*max(pointScale, vlen*0.016);	// assume 60hz
		l = normalize(cross(u, vec3(0.0, 0.0, -1.0)))*pointScale;
	}	
	
	{
		
		texCoord[1] = gl_TexCoordIn[0][1];	// vertex world pos (life in w)
		texCoord[2] = gl_TexCoordIn[0][2];	// vertex eye pos
		texCoord[3] = gl_TexCoordIn[0][3];	// vertex velocity in view space
		texCoord[3].w = fade;
		texCoord[4] = modelview*vec4(lightDir, 0.0);
		texCoord[4].w = gl_TexCoordIn[0][3].w; // attenuation
		texCoord[5].xyzw = gl_TexCoordIn[0][4].xyzw;	// color

		float zbias = 0.0f;//0.00125*2.0;

        texCoord[0] = vec4(0.0, 1.0, 0.0, 0.0);
        gl_Position = projection * vec4(p + u - l, 1.0);
		gl_Position.z -= zbias;
        EmitVertex();
		
		texCoord[0] = vec4(0.0, 0.0, 0.0, 0.0);
        gl_Position = projection * vec4(p - u - l, 1.0);
		gl_Position.z -= zbias;
        EmitVertex();

		texCoord[0] = vec4(1.0, 1.0, 0.0, 0.0);
        gl_Position = projection * vec4(p + u + l, 1.0);
		gl_Position.z -= zbias;
        EmitVertex();

		texCoord[0] = vec4(1.0, 0.0, 0.0, 0.0);
        gl_Position = projection * vec4(p - u + l, 1.0);
		gl_Position.z -= zbias;
        EmitVertex();
    }
}