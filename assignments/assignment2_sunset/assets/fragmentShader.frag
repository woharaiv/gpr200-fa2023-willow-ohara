#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec3 _Color;
uniform float _Brightness;
uniform float _Time;
uniform vec2 _Resoultion;
void main()
{
	FragColor = vec4(_Color * _Brightness * vec3(UV, 1.0),1.0);
}