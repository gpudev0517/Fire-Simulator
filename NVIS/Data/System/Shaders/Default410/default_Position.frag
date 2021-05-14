#version 410 core

in vec4 color;
layout(location = 0) out vec4 FragColor;

uniform int kindOIT; // 0 : None, 1 : WeightedBlended, 2 : DepthPeeling Init, 3 : DepthPeeling Peel.
uniform sampler2DRect depthPeelingDepthTex;


#define MAXCLIP 32
in vec4 modelPos;

uniform int nClipPlanes;
uniform vec4 ClipPlanes[MAXCLIP];
uniform bool isProcess[MAXCLIP];

uniform bool isPicked;
uniform bool isClipMode = false;

bool  isClip(vec4 worldPos)
{
	for(int i = 0; i < nClipPlanes; i++)
	{
		if(ClipPlanes[i].x*worldPos.x + ClipPlanes[i].y*worldPos.y + ClipPlanes[i].z*worldPos.z + ClipPlanes[i].w*worldPos.w < 0 && isProcess[i])
			return true;	
	}
	return false;
}

void main()
{
	if(isClipMode == true){
		if(isClip(modelPos))
			discard;
	}

    if(kindOIT == 2){
		FragColor = vec4(color.rgb  * color.a, 1.0 - color.a);
	}
	else if(kindOIT == 3){
		float frontDepth = texture(depthPeelingDepthTex, gl_FragCoord.xy).r;
		if(gl_FragCoord.z <= frontDepth)
			discard;
		FragColor = vec4(color.rgb  * color.a, color.a);		
	}
	else{
		FragColor = color;
	}
}