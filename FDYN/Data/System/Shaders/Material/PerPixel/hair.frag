// Computations are done in the eye space (Eye direction is (0,0,-1))

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4 Color;
uniform float Shininess;
uniform bool TwoSided;

float sinApprox(vec3 a, vec3 b)
{
	float x = dot(a, b);
	return sqrt(1 - x*x);
}

// rgb blond: 175, 143, 108

varying vec3 vertex;
varying vec3 texcoord0;
// varying float texcoord1;

// all computations are performed in eyespace
vec4 ShadingModel(vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, mat3 viewRot, vec3 materialValue)
{
	N = normalize(N);	// hair surface normal
	vec3 T = normalize(texcoord0);	// hair tangent
	
	// diffuse term: blend between Kajiya/Kay and lambert
	float diffuse = (1 - kDiffuse) * max(0, sinApprox(T, L)) + kDiffuse * max(0, dot(N, L));
	
	// eyeDir: not sure whether this should be negative - looks better this way!
	vec3 E = normalize(vertex);
	
	// Kajiya/Kay specular
	float specular = dot(T, L) * dot(T, E) + sinApprox(T, L) * sinApprox(T, E);
	specular = clamp(specular, 0, 1);
	specular = pow(specular, 128);
	
	return vec4(vertexColor.xyz * (clamp(kAmbient + diffuse, 0, 1)) + vec3(kSpecular, kSpecular, kSpecular) * specular, vertexColor.a);
}

vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, mat3 viewRot, vec3 materialValue, vec3 localPos)
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
