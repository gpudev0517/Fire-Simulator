
/*
	The lights for the shaders are filled and maintained by FLRenderer
*/

struct SpotLight
{
	vec3 pos;
	vec3 dir;
	vec4 color;
	float intensity;
	float cosCutoff;
	float exponent;
	float attLin;
	float attQuad;
};

struct PointLight
{
	vec3 pos;
	vec4 color;
	float intensity;
	float attLin;
	float attQuad;
};

struct DirLight
{
	vec3 dir;
	vec4 color;
	float intensity;
};

//Variables set by vertex shader
varying vec3 vertex;	// vertex position in camera space
varying vec4 vertexColor;	// vertex color

//forward declaration, ShadingModel is implemented by the material shader, e.g. phong.frag
vec4 ShadingModel(vec3 N, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 PointSpritesNormal);

float calculateAttenuation(in float dist, in float attLin, in float attQuad)
{
	return (1.0 / (1.0 + attLin * dist + attQuad * dist * dist));
}


/*
	Directional Light
*/
#if nDirLights > 0

uniform DirLight DirLights[nDirLights];


vec4 directionalLight(in int i, in vec3 N, in vec3 PointSpritesNormal)
{
	vec3 L = DirLights[i].dir;

	return ShadingModel(N, L, DirLights[i].color, DirLights[i].intensity, vertexColor, PointSpritesNormal);
}

#endif

int getNrOfDirLights()
{
	return nDirLights;
}

vec3 DirLightDirection(int i)
{
#if nDirLights > 0
	return DirLights[i].dir;
#else
	return vec3(0);
#endif
}

vec4 DirLightColor(int i)
{
#if nDirLights > 0
	return DirLights[i].color;
#else
	return vec4(0);
#endif
}
/*
	Point Light
*/
#if nPointLights > 0

uniform PointLight PointLights[nPointLights];


vec4 pointLight(in int i, in vec3 N, in vec3 PointSpritesNormal)
{
	vec3 D = PointLights[i].pos - vertex;
	vec3 L = normalize(D);

	float dist = length(D);
	float attenuation = calculateAttenuation(dist, PointLights[i].attLin, PointLights[i].attQuad);
	attenuation = 1.0;
	return attenuation * ShadingModel(N, L, PointLights[i].color, PointLights[i].intensity, vertexColor, PointSpritesNormal);
}

#endif

/*
	Spot Light
*/
#if nSpotLights > 0

uniform SpotLight SpotLights[nSpotLights];

vec4 spotLight(in int i, in vec3 N, in vec3 PointSpritesNormal)
{
	vec3 D = SpotLights[i].pos - vertex;
	vec3 L = normalize(D);
	vec4 black = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 tmpColor = ShadingModel(N, L, SpotLights[i].color, SpotLights[i].intensity, vertexColor, PointSpritesNormal);

	if(tmpColor.xyz != black.xyz)
	{
		float spotEffect = dot(SpotLights[i].dir, -L);

		if(spotEffect > SpotLights[i].cosCutoff)
		{
			float dist = length(D);
			float attenuation = calculateAttenuation(dist, SpotLights[i].attLin, SpotLights[i].attQuad);
			attenuation *= pow(spotEffect, SpotLights[i].exponent);
			return attenuation * tmpColor;
		}
	}
	return black;
}

#endif

vec4 calculateLighting(in vec3 N, in vec3 PointSpritesNormal)
{
	vec4 color = vec4(0.0);
	int i;

#if nDirLights > 0
	for(i = 0; i < nDirLights; ++i)
		color += directionalLight(i, N, PointSpritesNormal);
#endif
#if nPointLights > 0
	for(i = 0; i < nPointLights; ++i)
		color += pointLight(i, N, PointSpritesNormal);
#endif
#if nSpotLights > 0
	for(i = 0; i < nSpotLights; ++i)
		color += spotLight(i, N, PointSpritesNormal);
#endif

	color = clamp(color, 0.0, 1.0);

	return color;
}
