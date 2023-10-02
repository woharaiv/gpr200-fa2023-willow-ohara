#version 450

out vec2 UV;
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

uniform float _Scale;
uniform vec2 _PosOffset;

void main(){
	UV = vUV;
	gl_Position = vec4(vPos*_Scale,1.0);
	gl_Position += vec4(_PosOffset, 0.0, 0.0);
}