

#if nClipPlanes > 0
uniform vec4 ClipPlanes[nClipPlanes];
uniform bool isProcess[nClipPlanes];
#endif

bool  isClip(vec4 worldPos)
{
#if nClipPlanes > 0
	for(int i = 0; i < nClipPlanes; i++)
	{
		if(ClipPlanes[i].x*worldPos.x + ClipPlanes[i].y*worldPos.y + ClipPlanes[i].z*worldPos.z + ClipPlanes[i].w*worldPos.w < 0 && isProcess[i])
			return true;	
	}
#endif
	return false;
}

