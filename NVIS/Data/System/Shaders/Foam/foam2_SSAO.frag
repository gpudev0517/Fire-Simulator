#version 120
/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader applies Screen Space Ambient Occlusion to foam. 
 * Further, the foam's intensities are modified using the foam's density values.
 * It only runs when the data is correctly assigned via textures. In this textures,
 * the foam's eye space depth values and it's accumulated intensities have to be provided.
 * 
 * The SSAO principle is to create samples in a sphere in eye space uniformly. For each sample
 * where the depth values of the foam, the mesh or a rigid on the sample's  screen space
 * position is lower than the sample's depth, an occlusion is detected so the sample 
 * contributes to the fragment's shadow.
 * 
 */



 // Point size in world space
uniform float pointRadius;  
uniform float nearClip;
uniform float farClip;
// Screen width
uniform float width;
// Screen height
uniform float height;

// Modifies the intensitiy of the fragment
uniform float densityModifier;
// Modifies the densities exponentially
uniform float densityModifierExp;
// The initial search radius
uniform float searchRadius;
// The radius increment per SSAO pass
uniform float radiusStepSize;
// Multiplier on the number of samples genereated
uniform float samplingDensity;
// Minimum number of samples per fragment
uniform float minSamples;
// Maximum number of samples per fragment
uniform float maxSamples;
// The number of SSAO pases
uniform float passes;
// The maximum depth distance for shadowing in world space. Shadows fade quadratically over this 
uniform float maxOcclusionDistance;
// This is added to the final shadow value. Can be used to remoe self-occlusion
uniform float shadowOffset;
// Scales the final shadow value linearlyy
uniform float shadowScale;
// Scales the final shadow value exponentially
uniform float shadowExponent;
// The fluid's transparency. 0 means opaque fluid.
uniform float fluidTransparency;
// The maximum number of the incoming random numbers used for Halton Sampling.
uniform float RAND_MAX;

uniform mat3 invNormalMatrix;

// Foam intensities
//uniform sampler2D tex0;
// Foam depth information
uniform sampler2D tex0;
uniform sampler2D tex2;

// Environment texture 
uniform sampler2D tex5;
// World space normal
uniform sampler2D tex6;

// Mesh depth information
//uniform sampler2D tex2;
// Background picture
//uniform sampler2D tex3;
// Rigid depth information
//uniform sampler2D tex4;
// Modified intensities
//uniform sampler2D tex6;

// Fluid and rigid eye depth
//uniform sampler2D tex5;

const float PI = 3.141592654;
const float InvPI = 1.0/PI;
const float Inv2PI = 1.0/(2.0*PI);


// Calculates the eye space depth value from a depth buffer value
float eyeDepth(float z_n)
{
	return 2.0*farClip*nearClip/(z_n*farClip - farClip - nearClip - nearClip*z_n);
}

// Halton sampling generates quite uniform samples
vec3 haltonSampling(float k)
{
	k = floor(k);
	float PHI0 = 0.0, PHI1 = 0.0, PHI2 = 0.0, l = k, a = 0.0;
	int p0 = 2, p1 = 7, p2 = 13;
	int q = p0;
	// X
	while(l > 0)
	{
		a = float(mod(l, p0));
		PHI0 += a/float(q);
		l = int(l/p0);
		q *= p0;
	}
	// Y
	q = p1;
	l = k;
	while(l > 0)
	{
		a = float(mod(l, p1));
		PHI1 += a/float(q);
		l = int(l/p1);
		q *= p1;
	}
	// Z
	q = p2;
	l = k;
	while(l > 0)
	{
		a = float(mod(l, p2));
		PHI2 += a/float(q);
		l = int(l/p2);
		q *= p2;
	}
	return vec3(PHI0*2.0 - 1.0, PHI1*2.0 - 1.0, PHI2*2.0 - 1.0);
}


mat3 computeTripodMatrix(const vec3 direction)
{			
	const vec3 up = vec3(0.01, 0.99, 0.0);
	vec3 tangent = normalize(cross(up, direction));
	vec3 cotangent = normalize(cross(direction, tangent));
	return mat3(tangent, cotangent, direction);
}



// Main function
void main()
{	
	const float PI = 3.14159265359;

	float oneOverMaxOcclusionDistance = 1.0 / maxOcclusionDistance;

	// Get screen coordinates

	float invWidth = 1.0/width;
	float invHeight = 1.0/height;

	vec2 screenCoord = vec2(gl_FragCoord.x*invWidth, gl_FragCoord.y*invHeight);

	vec4 tex0Val = texture2D(tex0, screenCoord);
	
	// Get eye space depths of foam, rigid and fluid mesh for the current frament position
	float depthEye_mesh = tex0Val.x;//-eyeDepth(depth_mesh*2.0 - 1.0);
	float depthEye_rigid = tex0Val.y;//-eyeDepth(depth_rigid*2.0 - 1.0);
	float depthEye_foam = tex0Val.z;

	
	float shadow = 0.0, shadowPass = 0.0, shadowPassOccl = 0.0, intensity = 0.0;

	vec3 envColor = vec3(0,0,0);
	float totalIters = 1.0;
	intensity = texture2D(tex2, screenCoord).w;
	
	if(depthEye_foam > 0.0 || depthEye_rigid > 0.0)
	{

		float depthEye_foamSample = 0.0, depthEye_rigidSample = 0.0, depthEye_meshSample = 0.0;
		
		// SSAO variables
		float intensitySample = 0.0;
		float contribution = 0.0; 
		// The totalContribution accumulates the maximal contribution of all samples
		float totalContribution = 0.0;
		// Used to detect if samples are outside the screen
		vec2 sampleCoordPixel = vec2(0.0, 0.0);	
		// The search radius was calculated in the depth shader and is now brought to pixel values
		float searchRadiusPixel = 0.0;
		// The samples coordinates and their square
		vec3 sampleCoord, sampleCoord2;
		// The sample's depth values
		float depth_FoamSample, depth_rigidSample, depth_meshSample;
		float distFade = 1.0;
		
		// The random seed for Halton sampling
		//float seed = texture2D(tex6, screenCoord).x*RAND_MAX;
		float seed = 0.0;
		
		//		
		// SSAO for foam
		//
		// If the foam is visible
		if((depthEye_foam <= depthEye_rigid) && depthEye_foam > 0.0)
		{
			//
			// Calculate intensity
			//

			/*texture2D(tex0, screenCoord).w;
			if(densityModifier + intensity > 0.0)
			{
				intensity = pow(intensity, densityModifierExp);
				intensity = intensity/(densityModifier + intensity);
			}*/
			
			searchRadiusPixel = max(tex0Val.w*width*0.5, 1.0)*searchRadius;
			// The pass radius scales with the SSAO passes
			float passRadius = searchRadius;
			float passSearchRadiusPixel = searchRadiusPixel;
			
			// The sample Level is the number of samples generates. It responds to the sphere's totalContribution
			float passSearchRadiusPixel3 = searchRadiusPixel*searchRadiusPixel*searchRadiusPixel;
			float sampleLevel = min(max(passSearchRadiusPixel3*PI*0.75*samplingDensity, minSamples), maxSamples);


			vec3 normal_world = texture2D(tex6, screenCoord).xyz;

			mat3 localMatrix = computeTripodMatrix(normal_world);

			totalIters = 0.0;

			
			// Iterate over SSAO passes
			for(float pass = 1.0; pass <= passes; ++pass)
			{
				//int edgeSamples = ceil(pow(sampleLevel, 0.333333333));
				//sampleLevel = edgeSamples*edgeSamples*edgeSamples;
				//float samplingSize = 2.0/edgeSamples;

				totalContribution = 0.0;
				shadowPass = 0.0;

				//float ix = -1.0;
				//float iy = -1.0;
				//float iz = -1.0;
				
				// Iterate over the samples

				/*for(int i = 0; i<edgeSamples; i++)
					for(int j = 0; j<edgeSamples; j++)
						for(int k = 0; k<edgeSamples; k++)
						{
							sampleCoord.x = (float)i*samplingSize-1.0;
							sampleCoord.y = (float)j*samplingSize-1.0;
							sampleCoord.z = (float)k*samplingSize-1.0;*/
						


				for(float sample = 0.0; sample < sampleLevel; ++sample)
				{				
					// Create a uniform sample in ([-1, 1], [-1, 1], [-1, 1])
					sampleCoord = haltonSampling(seed++);
					
					/*
					sampleCoord.x = ix;
					sampleCoord.y = iy;
					sampleCoord.z = iz;

					ix += samplingSize;

					if(ix > 1.0)
					{
						ix = -1.0;
						iy += samplingSize;
					}

					if(iy > 1.0)
					{
						ix = -1.0;
						iy = -1.0;
						iz += samplingSize;
					}*/


					// The squared components
					sampleCoord2.x = sampleCoord.x*sampleCoord.x;
					sampleCoord2.y = sampleCoord.y*sampleCoord.y;
					sampleCoord2.z = sampleCoord.z*sampleCoord.z;
					
					// Transform to sphere
					sampleCoord.x = sampleCoord.x*sqrt(1.0 - sampleCoord2.y*0.5 - sampleCoord2.z*0.5 + sampleCoord2.y*sampleCoord2.z*0.33333333);
					sampleCoord.y = sampleCoord.y*sqrt(1.0 - sampleCoord2.z*0.5 - sampleCoord2.x*0.5 + sampleCoord2.z*sampleCoord2.x*0.33333333);
					sampleCoord.z = sampleCoord.z*sqrt(1.0 - sampleCoord2.x*0.5 - sampleCoord2.y*0.5 + sampleCoord2.x*sampleCoord2.y*0.33333333);
					
					// The sample's contribution is the squared distance
					contribution = 1.0 - sqrt(sampleCoord2.x + sampleCoord2.y + sampleCoord2.z);
					contribution *= contribution;
					
					// Accumulate the maximal contribution for all samples
					totalContribution += contribution;
					
					// Make the sample position fit a pixel in search radius
					sampleCoord.x = ceil(sampleCoord.x*passSearchRadiusPixel);
					sampleCoord.y = ceil(sampleCoord.y*passSearchRadiusPixel);
					// The z coordinate will be the eye space depth of the sample
					sampleCoord.z = depthEye_foam - sampleCoord.z*pointRadius*passRadius;
					
					sampleCoordPixel.x = sampleCoord.x + gl_FragCoord.x;
					sampleCoordPixel.y = sampleCoord.y + gl_FragCoord.y;
					// Case: Sample is inside the screen
					if(sampleCoordPixel.x > 0.0 && sampleCoordPixel.x < width && 
					   sampleCoordPixel.y > 0.0 && sampleCoordPixel.y < height)
					{

						// If this is positive, fragment normal and the sample are looking at the same direction

						vec3 worldSample = localMatrix*sampleCoord;
						vec3 worldSampleNormalized = normalize(worldSample);
						float SNDot = dot(worldSampleNormalized, normal_world);

						if(SNDot > 0.0)
						{
							//vec2 envCoord;
							//vec3 worldSampleCoord = invNormalMatrix * sampleCoord;
							//worldSampleCoord = normalize(worldSampleCoord);

							float theta = acos(worldSampleNormalized.y);
							float phi = atan(worldSampleNormalized.z, worldSampleNormalized.x);
				
							if (phi < 0.0) phi += 2.0*PI;
							if (phi > 2.0*PI) phi -= 2.0*PI;

							envColor += texture2D(tex5, vec2( phi / (2.0*PI), 1.0 - theta / PI )).xyz*SNDot;
							totalIters += SNDot;
						}


						// Switch to texture coordinates
						sampleCoord.xy = vec2(sampleCoordPixel.x*invWidth, sampleCoordPixel.y*invHeight);
						
						// Get the samples intensity value
						intensitySample = texture2D(tex2, sampleCoord.xy).w;
							
						vec4 tex0ValSample = texture2D(tex0, sampleCoord.xy);
						// Get the sample's depth values and test if there any depth value
						depthEye_foamSample = tex0ValSample.z;
						
						if(depthEye_foamSample == 0.0) 
							depthEye_foamSample = 999999.9;
						

						//vec4 tex5ValSample = texture2D(tex5, sampleCoord.xy);

						depthEye_rigidSample = tex0ValSample.y;//-eyeDepth(texture2D(tex4, sampleCoord.xy).x*2.0 - 1.0);

						if(depthEye_rigidSample == 0.0) 
							depthEye_rigidSample = 999999.9;
						

						depthEye_meshSample = tex0ValSample.x;//-eyeDepth(texture2D(tex2, sampleCoord.xy).x*2.0 - 1.0);
						
						// Indices: 0 - Foam, 1 - Rigid, 2 - Mesh
						float depthSample[3] = float[](depthEye_foamSample, depthEye_rigidSample, depthEye_meshSample);
						float depthMaterials[3] = float[](intensitySample, 1.0, fluidTransparency);
						
						// Loop over the 3 possible occlusions
						shadowPassOccl = 0.0;
						for(int occl = 0; occl < 3; ++occl)
						{
							if(depthSample[occl] < sampleCoord.z)
							{
								// Calculate the normalized depth difference between sample and geometry and take all from [0,1]
								distFade = 1.0 - abs(depthSample[occl] - sampleCoord.z)*oneOverMaxOcclusionDistance;
								if(distFade > 0.0)
								{
									// Accumulate the shadow
									shadowPassOccl += contribution*distFade*distFade*depthMaterials[occl];
								}
							}
						}
						shadowPass += min(shadowPassOccl, contribution);
					}
					
				}
				// Accumulate the passes' shadow values
				if(totalContribution > 0.0)
					shadow += shadowPass/totalContribution;
				
				// Increase the pass radius
				passRadius += radiusStepSize;
				passSearchRadiusPixel = searchRadiusPixel * passRadius;
				passSearchRadiusPixel3 = passSearchRadiusPixel*passSearchRadiusPixel*passSearchRadiusPixel;
				// Increase the sample level
				sampleLevel = min(max(passSearchRadiusPixel3*PI*samplingDensity, minSamples), maxSamples);
			}
			// Build the average of the passes shadow values
			if(passes > 0.0)
				shadow /= passes;
		}	
		//
		// SSAO for rigids
		//
		/*else if(depth_rigid < 1.0)
		{
			searchRadiusPixel = 
			searchRadiusPixel	= max(texture2D(tex4, screenCoord).z*width*0.5, 1.0)*searchRadius;
			// The pass radius scales with the SSAO passes
			float passRadius = searchRadius;
			float passSearchRadiusPixel = searchRadiusPixel;
			
			// The sample Level is the number of samples generates. It responds to the sphere's totalContribution
			float searchRadiusPixel3 = searchRadiusPixel;
			searchRadiusPixel3 = searchRadiusPixel3*searchRadiusPixel3*searchRadiusPixel3;
			float sampleLevel = min(max(searchRadiusPixel3*PI*0.75*samplingDensity, minSamples), maxSamples);
			
			// Iterate over SSAO passes
			for(float pass = 1.0; pass <= passes; ++pass)
			{
				totalContribution = 0.0;
				shadowPass = 0.0;
				
				// Iterate over the samples
				for(float sample = 0.0; sample < sampleLevel; ++sample)
				{				
					// Create a uniform sample in ([-1, 1], [-1, 1], [-1, 1])
					sampleCoord = haltonSampling(seed++);
					// The squared components
					sampleCoord2.x = sampleCoord.x*sampleCoord.x;
					sampleCoord2.y = sampleCoord.y*sampleCoord.y;
					sampleCoord2.z = sampleCoord.z*sampleCoord.z;
					
					// Transform to sphere
					sampleCoord.x = sampleCoord.x*sqrt(1.0 - sampleCoord2.y*0.5 - sampleCoord2.z*0.5 + sampleCoord2.y*sampleCoord2.z*0.33333333);
					sampleCoord.y = sampleCoord.y*sqrt(1.0 - sampleCoord2.z*0.5 - sampleCoord2.x*0.5 + sampleCoord2.z*sampleCoord2.x*0.33333333);
					sampleCoord.z = sampleCoord.z*sqrt(1.0 - sampleCoord2.x*0.5 - sampleCoord2.y*0.5 + sampleCoord2.x*sampleCoord2.y*0.33333333);
					
					// The sample's contribution is the squared distance
					contribution = 1.0 - sqrt(sampleCoord2.x + sampleCoord2.y + sampleCoord2.z);
					contribution *= contribution;
					
					// Accumulate the maximal contribution for all samples
					totalContribution += contribution;
					
					// Make the sample position fit a pixel in search radius
					sampleCoord.x = ceil(sampleCoord.x*searchRadiusPixel);
					sampleCoord.y = ceil(sampleCoord.y*searchRadiusPixel);
					// The z coordinate will be the eye space depth of the sample
					sampleCoord.z = depthEye_rigid - sampleCoord.z*pointRadius*passRadius;
					
					sampleCoordPixel.x = sampleCoord.x + gl_FragCoord.x;
					sampleCoordPixel.y = sampleCoord.y + gl_FragCoord.y;
					// Case: Sample is inide the screen
					if(sampleCoordPixel.x > 0.0 && sampleCoordPixel.x < width && 
					   sampleCoordPixel.y > 0.0 && sampleCoordPixel.y < height)
					{
						// Switch to texture coordinates
						sampleCoord.xy = vec2(sampleCoordPixel.x/width, sampleCoordPixel.y/height);
						
						// Get the samples intensity value
						intensitySample = texture2D(tex0, sampleCoord).w;
						if(densityModifier + intensitySample > 0.0)
						{
							intensitySample = pow(intensitySample, 1.25);
							intensitySample = intensitySample/(densityModifier + intensitySample);
						}
							
						// Get the sample's depth values and test if there is one 
						depthEye_foamSample = texture2D(tex0, sampleCoord.xy).y;
						if(depthEye_foamSample == 0.0) depthEye_foamSample = 999999.9;					
						
						// Only foam may contribute shadow to rigids
						if(depthEye_foamSample < sampleCoord.z)
						{
							// Calculate the normalized depth difference between sample and geometry and take all from [0,1]
							distFade = 1.0 - abs(depthEye_foamSample - sampleCoord.z)/maxOcclusionDistance;
							if(distFade > 0.0)
							{
								// Accumulate the shadow
								//shadowPass += contribution*distFade*distFade*intensitySample;
								shadowPass += contribution*distFade*distFade*intensitySample;
							}
						}
					}
				}
				// Accumulate the passes' shadow values
				if(totalContribution > 0.0)
					shadow += shadowPass/totalContribution;
				
				// Increase the pass radius
				passRadius += radiusStepSize;
				passSearchRadiusPixel = searchRadiusPixel * passRadius;
				passSearchRadiusPixel = passSearchRadiusPixel*passSearchRadiusPixel*passSearchRadiusPixel;
				// Increase the sample level
				sampleLevel = min(max(passSearchRadiusPixel*PI*samplingDensity, minSamples), maxSamples);
			}
			// Build the average of the passes shadow values
			if(passes > 0.0)
				shadow /= passes;
		}*/
		
		// Negate the lower shadow totalContributions and scale the remaining to [0, 1]
		// This is useful to eliminate selfshadowing and to increase the shadow contrast
		shadow = pow(shadow, shadowExponent);
		shadow = shadow/(1.0 + shadowOffset);
		shadow *= shadowScale;
		shadow = max(min(shadow + shadowOffset, 1.0), 0.0);
		
	}
	
	// Save values into FBO texture for the filter
	vec4 color = vec4(shadow, intensity, 0.0, 0.0);		
	//vec4 color = vec4(1.0, intensity, 0, 0);	
	gl_FragData[0] = color;

	gl_FragData[1] = vec4(envColor/totalIters, 1.0);
	return;
}
