// Computations are done in the eye space (Eye direction is (0,0,-1))

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4  Color;
uniform float Shininess;
uniform bool  TwoSided;

uniform int screenWidth;
uniform int screenHeight;

uniform sampler2D sceneTex;

const float Eta = 0.15; // Water

float cube(float x) { return x*x*x; }

vec4 ShadingModel(vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue)
{
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
	vec4 lightedColor = Color * lightColor * vertexColor;
	vec3 transmission = 1.0-(1.0-vertexColor.xyz)*1.0*0.8;
	vec3 refract = texture2D(sceneTex, gl_FragCoord / vec2(screenWidth, screenHeight)).xyz;
	vec4 reflect = vec4(1.0) * 1.0f;
	vec3 v = -normalize(P);

	float fresnel = cube(1.0-max(dot(N, v), 0.0));
	vec4 opaqueColor = vec4(mix(refract, lightedColor.rgb, lightedColor.a), 1.0f);
	vec4 finalColor = mix(opaqueColor, reflect, fresnel);
	
	return kDiffuse * diff * finalColor + kAmbient * finalColor + kSpecular * spec * lightColor * intensity;
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
