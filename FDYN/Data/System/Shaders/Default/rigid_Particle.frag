#version 140

uniform sampler2DRect particlePCMapTexture;
uniform sampler2DRect particlePCMapFlatWorld;
uniform float PCMapWidth;
uniform float PCMapHeight;

#define STEP 5.0


//	Trilinear interpolation Algorithm
// 	https://en.wikipedia.org/wiki/Trilinear_interpolation
vec4 rigidParticleColor(vec3 fragWorldPos)
{
	vec2 fragCoord = gl_FragCoord.xy;
	vec3 fragColor = texture(particlePCMapTexture, fragCoord).xyz;
	
	vec4 inputW = texture(particlePCMapFlatWorld, fragCoord);
		
	int vertexID = int(inputW.r);
	vec3 flatWorldPos = inputW.gba;
	
	
	vec3 worldPoses[9];
	vec3 fragColors[9];
	int vertexIDs[9];
	
	worldPoses[0] = flatWorldPos;
	fragColors[0] = fragColor;
	vertexIDs[0] = vertexID;
	
	vec2 newFragCoord = fragCoord;
	worldPoses[1] = flatWorldPos;
	fragColors[1] = fragColor;
	vertexIDs[1] = vertexID;
	
	while( newFragCoord.x < PCMapWidth - 1 )
	{
		newFragCoord.x += STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID)
		{
			worldPoses[1] = newInputW.gba;
			fragColors[1] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[1] = newVertexID;
			break;
		}
		// else if( newVertexID < 2)
			// break;
	}
	
	
	
	
	newFragCoord = fragCoord;
	worldPoses[2] = flatWorldPos;
	fragColors[2] = fragColor;
	vertexIDs[2] = vertexID;
	
	while( newFragCoord.x > 0 )
	{
		newFragCoord.x -= STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID)
		{
			worldPoses[2] = newInputW.gba;
			fragColors[2] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[2] = newVertexID;
			break;
		}
		// else if( newVertexID < 2)
			// break;
	}
	
	
	
	
	newFragCoord = fragCoord;
	worldPoses[3] = flatWorldPos;
	fragColors[3] = fragColor;
	vertexIDs[3] = vertexID;
	
	while( newFragCoord.y < PCMapHeight - 1 )
	{
		newFragCoord.y += STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID)
		{
			worldPoses[3] = newInputW.gba;
			fragColors[3] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[3] = newVertexID;
			break;
		}
		// else if( newVertexID < 2)
			// break;
	}
	
	
	
	newFragCoord = fragCoord;
	worldPoses[4] = flatWorldPos;
	fragColors[4] = fragColor;
	vertexIDs[4] = vertexID;
	
	while( newFragCoord.y > 0 )
	{
		newFragCoord.y -= STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID)
		{
			worldPoses[4] = newInputW.gba;
			fragColors[4] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[4] = newVertexID;
			break;
		}
		// else if( newVertexID < 2)
			// break;
	}
	
	
	newFragCoord = fragCoord;
	worldPoses[5] = flatWorldPos;
	fragColors[5] = fragColor;
	vertexIDs[5] = vertexID;
	
	while( newFragCoord.x < PCMapWidth - 1 && newFragCoord.y < PCMapHeight - 1 )
	{
		newFragCoord.x += STEP;
		newFragCoord.y += STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID && newVertexID != vertexIDs[1] && newVertexID != vertexIDs[3])
		{
			worldPoses[5] = newInputW.gba;
			fragColors[5] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[5] = newVertexID;
			break;
		}
		// else if( newVertexID < 2)
			// break;
	}
	
	newFragCoord = fragCoord;
	worldPoses[6] = flatWorldPos;
	fragColors[6] = fragColor;
	vertexIDs[6] = vertexID;
	
	while( newFragCoord.x < PCMapWidth - 1 && newFragCoord.y > 0 )
	{
		newFragCoord.x += STEP;
		newFragCoord.y -= STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID && newVertexID != vertexIDs[1] && newVertexID != vertexIDs[4])
		{
			worldPoses[6] = newInputW.gba;
			fragColors[6] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[6] = newVertexID;
			break;
		}
		// else if( newVertexID < 2)
			// break;
	}
	
	newFragCoord = fragCoord;
	worldPoses[7] = flatWorldPos;
	fragColors[7] = fragColor;
	vertexIDs[7] = vertexID;
	
	while( newFragCoord.x > 0 && newFragCoord.y < PCMapHeight - 1 )
	{
		newFragCoord.x -= STEP;
		newFragCoord.y += STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID && newVertexID != vertexIDs[2] && newVertexID != vertexIDs[3])
		{
			worldPoses[7] = newInputW.gba;
			fragColors[7] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[7] = newVertexID;
			break;
		}
	}
	
	
	newFragCoord = fragCoord;
	worldPoses[8] = flatWorldPos;
	fragColors[8] = fragColor;
	vertexIDs[8] = vertexID;
	
	while( newFragCoord.x > 0 && newFragCoord.y > 0 )
	{
		newFragCoord.x -= STEP;
		newFragCoord.y -= STEP;
		vec4 newInputW = texture(particlePCMapFlatWorld, newFragCoord);
		int newVertexID = int(newInputW.r);
		if(newVertexID >= 2 && newVertexID != vertexID && newVertexID != vertexIDs[2] && newVertexID != vertexIDs[4])
		{
			worldPoses[8] = newInputW.gba;
			fragColors[8] = texture(particlePCMapTexture, newFragCoord).rgb;
			vertexIDs[8] = newVertexID;
			break;
		}
	}
	
	
	
	vec3 v = fragWorldPos.xyz;

	float len00 = -1, len01 = -1, len10 = -1, len11 = -1;

	bool f0 = true, f1 = true;

	vec3 c00, c01, c10, c11, c0, c1, c;
	vec3 p00, p01, p10, p11, p0, p1;

	for (int r = 0; r < 9; r ++)
	{
		vec3 posXYZ = worldPoses[r];

		float posX = posXYZ.x;
		float posY = posXYZ.y;

		vec3 dis = v - posXYZ;
		float diff3 = dot(dis, dis);
		
		bool posXB = v.x <= posX;
		bool posYB = v.y <= posY;

		vec3 color = 255.0 * fragColors[r];
		if ( posXB ){
			if ( posYB ){
				if(len00 < 0 || len00 > diff3)
				{
					len00 = diff3;
					p00 = posXYZ;
					c00 = color;
					continue;
				}
			}
			else{
				
				if(len01 < 0 || len01 > diff3)
				{
					len01 = diff3;
					p01 = posXYZ;
					c01 = color;
					continue;
				}
				
			}
		}
		else
		{
			if ( posYB ){
				
				if(len10 < 0 || len10 > diff3)
				{
					len10 = diff3;
					p10 = posXYZ;
					c10 = color;
					continue;
				}
				
			}
			else{
				
				if( len11 < 0 || len11 > diff3)
				{
					len11 = diff3;
					p11 = posXYZ;
					c11 = color;
					continue;
				}
				
			}
		}
	}
	
	if( len00 >= 0 && len10 >= 0 ) {
		vec3 unit = normalize(p10 - p00);
		float xd = dot(v - p00, unit)/length(p10 - p00);
		c0 = c00 * (1.0 - xd) + c10 * xd;
		p0 = p00 * (1.0 - xd) + p10 * xd;
		//p0 = (p00 + p10) / 2.0;
		f0 = false;
	}
	else if( len00 >= 0 ) {
		c0 = c00;
		p0 = p00;
		f0 = false;
	}
	else {
		c0 = c10;
		p0 = p10;
		f0 = len10 < 0;
	}

	if( len01 >= 0 && len11 >= 0) {
		vec3 unit = normalize(p11 - p01);
		float xd = dot(v - p01, unit)/length(p11 - p01);
		c1 = c01 * (1.0 - xd) + c11 * xd;
		p1 = p01 * (1.0 - xd) + p11 * xd;
		//p1 = (p01 + p11) / 2.0;
		f1 = false;
	}
	else if( len01 >= 0 ) {
		c1 = c01;
		p1 = p01;
		f1 = false;
	}
	else {
		c1 = c11;
		p1 = p11;
		f1 = len11 < 0;
	}

	
	if( !f0 && !f1 ){
		vec3 unit = normalize(p1 - p0);
		float zd = dot(v - p0, unit)/length(p1 - p0);
		c = c0 * (1.0 - zd) + c1 * zd;
	}
	else if( !f0 )
		c = c0;
	else if( !f1 )
		c = c1;
	else 
		c = vec3(255.0, 255.0, 255.0);

	return vec4(c / 255.0, 1.0);
}