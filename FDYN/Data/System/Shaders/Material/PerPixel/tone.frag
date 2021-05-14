// Shades using the Gooch tone lighting model for NPR

uniform float kAmbient;
uniform float kDiffuse = 1;
uniform float kSpecular;

uniform vec4 Color;
uniform bool TwoSided;
uniform vec3 camPosition;

uniform float blueShift = 0.4;
uniform float blueLuminance = 0.4;
uniform float yellowShift = 0.2;
uniform float yellowLuminance = 0.6;


vec4 ShadingModel( vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue )
{

	vec3 halfVec = normalize(L + vec3(0.0, 0.0, 1.0));
	const float spec = 128.0;
	
	float NdotL = dot(N, L);
	float NdotH = dot(N, halfVec);
	
	vec4 specular = vec4(pow(max(0.0, NdotH), spec)) * kSpecular;
	
	vec4 kBlue = vec4(0.0, 0.0, clamp(blueShift, 0.0, 1.0), 1.0);
	vec4 kCool = kBlue + blueLuminance * kDiffuse;
	
	vec4 kYellow = vec4(clamp(yellowShift, 0.0, 1.0), clamp(yellowShift, 0.0, 1.0), 0.0, 1.0);
	vec4 kWarm = kYellow + yellowLuminance * kDiffuse;
	
	vec4 myColor = (((1.0 + NdotL) / 2.0) * kWarm + (1.0 - ((1.0 + NdotL) / 2.0)) * kCool)*vertexColor + specular;
	
	myColor.w = 0;
	//myColor = vec4( 1, 0, 0, 0 );
	return myColor;
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