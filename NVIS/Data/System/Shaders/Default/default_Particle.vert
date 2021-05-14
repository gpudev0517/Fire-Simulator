#version 130

out vec3  posEye;        // position of center in eye space
out float pointSize;
out vec4  vertexColor;
out vec4  worldPos;
flat out int   vertexid;

uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform bool lighting;
uniform bool opacity;
uniform bool picked; 		// render particle as selected
uniform bool isPicking; 	// if this rendering is color-code rendering for selection support
uniform bool ortho;			// if the camera does orthographic projection
uniform bool isPCMap;

uniform vec4 meshColor;

void main()
{
    // calculate window-space point size
    posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));
    float dist = abs(posEye.z);
    
    if(!ortho)
		gl_PointSize = pointRadius * (pointScale / dist);
	else
		gl_PointSize = pointRadius * pointScale;
	
	if(isPCMap == true)
		gl_PointSize *= 1;
		
    pointSize = gl_PointSize;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);

    if (isPicking){
		vertexColor = meshColor;
	}
	else
	{
		vertexColor = gl_Color;
		vertexColor.a = meshColor.a;
	}

	worldPos = vec4(gl_Vertex.xyz, 1.0);
	vertexid = gl_VertexID;
}