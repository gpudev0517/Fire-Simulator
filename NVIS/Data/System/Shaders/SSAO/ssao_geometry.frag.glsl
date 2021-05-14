#version 410 core


layout (location = 0) in vec4 Albedo;

layout(location=0) out vec4 out_Color;

void main()
{

    out_Color = Albedo;
}