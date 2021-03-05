// Computations are done in the eye space (Eye direction is (0,0,-1))

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4  Color;
uniform vec4  specColor;
uniform float Shininess;
uniform bool  TwoSided;
uniform bool  isFlat;

vec4 ShadingModel(vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue)
{
	if(isFlat == true)
		return vertexColor;
	
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
	vec4 shadedColor = kDiffuse * diff * finalColor + kAmbient * finalColor + kSpecular * spec * specColor * lightColor * intensity;
	return vec4(shadedColor.rgb, finalColor.a);
}

vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
	return vec3(0);
}

vec3 PostProcessing(vec3 color)
{
	return color;
}

vec4 PreProcessing(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
    return vertexColor;
}