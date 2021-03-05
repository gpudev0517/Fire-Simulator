// Shades using the terrain (rigid topography)

// Terrain coloring
uniform float minTerrainElevation;
uniform float maxTerrainElevation;

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4  Color;
uniform vec4  specColor;
uniform float Shininess;
uniform bool  TwoSided;
uniform bool  usePhongShading;

vec3 getInterpolatedColor(float s);

vec4 PreProcessing(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
    vec4 vColor;
	float terrainS = 0.0;
	if (maxTerrainElevation != minTerrainElevation)
	{
		if (localPos.y > maxTerrainElevation)
			terrainS = 1.0;
		else if (localPos.y > minTerrainElevation)
		{
			if (maxTerrainElevation > minTerrainElevation)
				terrainS = (localPos.y - minTerrainElevation) / (maxTerrainElevation - minTerrainElevation);
			else
				terrainS = (localPos.y - maxTerrainElevation) / (minTerrainElevation - maxTerrainElevation);
		}
	}
	vColor.rgb = getInterpolatedColor(terrainS);
	vColor.a = vertexColor.a;

    return vColor;
}

vec4 ShadingModel( vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue )
{
	if (usePhongShading == false)
		return vec4(0, 0, 0, 0);
//		return vertexColor;

	// L is the direction from the vertex to the light source
	L = normalize(L);
	float nDotL = dot(N, L);
	if(nDotL <= 0.0)
	{
		if(!TwoSided)
			return vec4(0, 0, 0, 1);
		else
			nDotL = -nDotL;
	}

	float diff = clamp(nDotL, 0.0, 1.0);
	vec3 R = reflect(-L, N);
	float spec = pow(clamp(dot(vec3(0.0, 0.0, 1.0), R), 0.0, 1.0), Shininess) * nDotL;
	vec4 finalColor = Color * lightColor * vertexColor * intensity;
	vec4 shadedColor = kDiffuse * diff * finalColor + kAmbient * finalColor + kSpecular * spec * specColor * lightColor;
	return vec4(shadedColor.rgb, finalColor.a);
}

vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
	if (usePhongShading == false)
		return vertexColor.rgb;
	else
		return vec3(0);
}

vec3 PostProcessing(vec3 color)
{
	return color;
}