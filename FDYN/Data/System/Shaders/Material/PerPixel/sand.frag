// Computations are done in the eye space (Eye direction is (0,0,-1))

//uniform float kAmbient;
//uniform float kDiffuse;
//uniform float kSpecular;
//uniform vec4 Color;
//uniform float Shininess;
//uniform bool TwoSided;


vec4 ShadingModel(vec3 N, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 PointSpritesNormal, mat3 viewRot, vec3 materialValue)
{
    vec4 diffuse = vertexColor;
    vec4 amb = vertexColor*0.18;
    vec4 ambient = vec4(0.1, 0.4, 0.5, 1.0)*.2;
    diffuse = diffuse;//+ambient;	    
    
    vec4 lightCol = lightColor;
    vec3 lightDir = normalize(L);
    
    /****** sphere shading facing to camera ******/
    
    //if(!TwoSided)      
	//	float NdotL = max(0.0, dot(lightDir, PointSpritesNormal));
    //else
    //{
		float NdotL = dot(lightDir, PointSpritesNormal);    
		if(NdotL <= 0.0)
			NdotL = -NdotL;
	//}
	
	NdotL = clamp(NdotL, 0.0, 1.0);
    diffuse = diffuse * lightColor * NdotL * intensity;
    diffuse = diffuse*0.55; 
    
    //Contrast Enhancement
	float corr1 = 1.0-NdotL;
	if(NdotL <= 0.3)		
		diffuse = diffuse - vec4(corr1*0.05, corr1*0.05, corr1*0.05, 1.0);      
    
    /****** shading according to the normal of the sand pile structure ******/
        
	N.z = PointSpritesNormal.z;
	N = normalize(N);
	
	//if(!TwoSided)	
	//	float NdotL1 = max(0.0, dot(lightDir, N));
	//else
	//{
		float NdotL1 = dot(lightDir, N);   
		if(NdotL1 <= 0.0)
			NdotL1 = -NdotL1;
	//}
		
	NdotL1 = clamp(NdotL1, 0.0, 1.0);	
	if (NdotL1 > 0.0){
		diffuse += (diffuse * NdotL1)*1.3;
	}	
	
	//Contrast Enhancement
	float corr = 1.0-NdotL1;
	if(NdotL1 <= 0.3)		
		diffuse = diffuse - vec4(corr*0.05, corr*0.05, corr*0.05, 1.0);

	return amb + diffuse;	
   
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