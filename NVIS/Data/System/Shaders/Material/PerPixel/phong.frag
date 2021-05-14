#version 130

// Computations are done in the eye space (Eye direction is (0,0,-1))

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4  Color;
uniform vec4  specColor;
uniform float Shininess;
uniform bool  TwoSided;
uniform bool  isFlat;


const float PI = 3.1415927;

// IBL
uniform bool environmented;
uniform float environmentScale = 1.0;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

uniform mat3 viewRot;


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

/* vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
	return vec3(0);
} */

// The Fresnel equation is a rather complex equation, but luckily the Fresnel equation can be approximated using the Fresnel-Schlick approximation: 
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// Given F, the remaining terms to calculate are the normal distribution function D and the geometry function G. 
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
	vec3 V = normalize(-P);
	vec3 R = reflect(-V, N);
	vec3 albedo = vertexColor.xyz;

    float roughness = materialValue.r;
    float metallic = materialValue.g;
    float ao = materialValue.b;
	
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 ambient = vec3(kAmbient) * albedo * ao;
    if (environmented)
    {
        vec3 irradiance = texture(irradianceMap, transpose(viewRot)*N).rgb;
        vec3 diffuse = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 5.0;
        vec3 prefilteredColor = textureLod(prefilterMap, transpose(viewRot)*R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
        
        // Ambient
        ambient += (kD * diffuse + specular) * ao * environmentScale;
    }
    
    return ambient;
}

vec3 PostProcessing(vec3 color)
{
	return color;
}

vec4 PreProcessing(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
    return vertexColor;
}