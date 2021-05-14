#version 330  compatibility

in vec4 tc0;
in vec4 worldPos;

out vec4 fragColor;

uniform float u_Xmin;
uniform float u_Xmax;
uniform float u_Zmin;
uniform float u_Zmax;

uniform sampler2D u_texture;
uniform vec4 meshColor;
uniform int useTexture;

vec4 calculateVisibility(in vec4 vertexPosition);

void main()
{
    if (tc0.s < u_Xmin || tc0.s > u_Xmax || tc0.t < u_Zmin || tc0.t > u_Zmax) {
        discard;
    } else {
		vec4 visibility = calculateVisibility(worldPos);
		if( useTexture == 1 ){
			fragColor = visibility * texture(u_texture, tc0.st);
		}
		else {
			fragColor = visibility * meshColor;
		}
    }
}
