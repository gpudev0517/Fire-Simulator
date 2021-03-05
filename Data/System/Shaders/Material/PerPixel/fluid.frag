#version 130

// Computations are done in the eye space (Eye direction is (0,0,-1))
in vec2 texCoord_clone;

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4  Color;
uniform float Shininess;
uniform bool  TwoSided;
uniform bool  isFlat;

uniform vec4  specColor;

uniform mat4 modelviewinv;

uniform sampler2D thicknessTex;
uniform sampler2D sceneTex;

const float Eta = 0.15; // Water

uniform float ior;
uniform float fresnelStrength;

float cube(float x) { return x*x*x; }

vec4 ShadingModel(vec3 n, vec3 P, vec3 l, vec4 lightColor, float intensity, vec4 color, vec3 materialValue)
{
	if(isFlat == true)
		return color;
	
	// L is the direction from the vertex to the light source
	/*L = normalize(L);
	float nDotL = max(0.0f, dot(N, L)) * 0.5 + 0.5;

	vec3 R = reflect(-L, N);
	float spec = pow(clamp(dot(vec3(0.0, 0.0, 1.0), R), 0.0, 1.0), Shininess) * nDotL;
	//return vec4(lightColor.rgb * vertexColor.rgb * nDotL, spec);*/
	
	vec4 worldPos = modelviewinv*vec4(P, 1.0);
	
	vec3 v = -normalize(P);
	
	vec3 h = normalize(v + l);
	
	vec3 skyColor = vec3(0.1, 0.2, 0.4)*1.2;
	vec3 groundColor = vec3(0.1, 0.1, 0.2);
	
	float fresnel = cube(1.0-max(dot(n, v), 0.0));
    
	float ln = dot(l, n);
    
	vec3 rEye = reflect(-v, n).xyz;
	vec3 rWorld = (modelviewinv*vec4(rEye, 0.0)).xyz;
	
	vec2 texScale = vec2(0.75, 1.0);	// to account for backbuffer aspect ratio (todo: pass in)
    
	float refractScale = ior*0.025;
	float reflectScale = ior*0.1;
    
	// attenuate refraction near ground (hack)
	refractScale *= smoothstep(0.1, 0.4, worldPos.y);
	
	vec2 refractCoord = texCoord_clone + n.xy*refractScale*texScale;	
	//vec2 refractCoord = texCoord_clone + refract(-v, n, 1.0/1.33)*refractScale*texScale;	
	
	// read thickness from refracted coordinate otherwise we get halos around objectsw
	float thickness = max(texture2D(thicknessTex, refractCoord).x, 0.3);
	
	//vec3 transmission = exp(-(vec3(1.0)-color.xyz)*thickness);
	vec3 transmission = 1.0-(1.0-color.xyz)*thickness*0.8;
	vec3 refract = texture2D(sceneTex, refractCoord).xyz*transmission;
	
	vec2 sceneReflectCoord = texCoord_clone - rEye.xy*texScale*reflectScale/P.z;
	vec3 sceneReflect = texture2D(sceneTex, sceneReflectCoord).xyz;
    
	// fade out planar reflections above the ground
	vec3 reflect = vec3(1.0) * fresnelStrength + sceneReflect * (1.0 - fresnelStrength);
	
	// lighting
	//vec3 diffuse = color.xyz*mix(vec3(0.29, 0.379, 0.59), vec3(1.0), (ln*0.5 + 0.5)*max(shadow, 0.4)) * (1.0-color.w);
	vec3 specular = vec3(1.2*pow(max(dot(h, n), 0.0), Shininess)) * specColor.rgb;
    
	vec3 opaqueColor = mix(refract, color.xyz, color.w);
	return vec4(/*diffuse +*/ mix(opaqueColor, reflect, fresnel) + specular, 1.0);
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
