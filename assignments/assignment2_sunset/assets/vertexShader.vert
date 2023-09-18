#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 inUV;
out vec2 UV;
void main()
{
	UV = inUV;
	gl_Position = vec4(vPos,1.0);
}