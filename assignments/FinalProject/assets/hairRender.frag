#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _HairMap;

uniform int _ShellNumber;

uniform float _BaseColorThreshold;
uniform float _ColorThresholdDecay;

void main(){
	//TODO: Add separate color property so hieght and brightness aren't tied (Maybe 2 textures?)
	FragColor = texture(_HairMap,fs_in.UV);
	if (length(FragColor) < (_BaseColorThreshold + _ColorThresholdDecay * _ShellNumber)) discard;
}