// Computations are done in the eye space (Eye direction is (0,0,-1))

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4  Color;
uniform float Shininess;
uniform bool  TwoSided;


vec4 ShadingModel(vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue)
{
	vec4 finalColor = vertexColor ;
	return finalColor;
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
