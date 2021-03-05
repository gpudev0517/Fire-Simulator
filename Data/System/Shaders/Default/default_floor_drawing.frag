#version 330  compatibility

varying vec4 gl_TexCoord[];

out vec4 fragColor;

uniform float u_Xmin;
uniform float u_Xmax;
uniform float u_Zmin;
uniform float u_Zmax;
uniform sampler2D u_texture;

void main()
{
    if (gl_TexCoord[0].s < u_Xmin || gl_TexCoord[0].s > u_Xmax || gl_TexCoord[0].t < u_Zmin || gl_TexCoord[0].t > u_Zmax) {
        discard;
    } else {
        fragColor = texture(u_texture, gl_TexCoord[0].st);
    }
}
