#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _HairMap;
uniform sampler2D _Texture;

uniform int _ShellNumber;
uniform int _ShellsRendering;

uniform float _ColorThresholdDecay;
uniform float _HairCutoffSlope;

uniform float _Attenuation;

void main(){
	vec2 hairMapSize = textureSize(_HairMap, 0);
	//how far is the current UV position from the bottom left of the hair map texel it sits on?
	vec2 subTexelUV = vec2(mod(fs_in.UV.x * hairMapSize.x, 1), mod(fs_in.UV.y * hairMapSize.y, 1));
	//How far is the current UV position from the center of the hair map texel is sits on?
	float distFromMapTexelCenter = distance(vec2(0.5, 0.5), subTexelUV);
	//Brighter texel means longer strand
	float height = length(texture(_HairMap,fs_in.UV));
	//Don't render the fragment if the strand is too tall or too wide
	if (height < (1 + _ColorThresholdDecay * _ShellNumber) || distFromMapTexelCenter > ((_HairCutoffSlope*_ShellsRendering/_ShellNumber))) discard;
	else //Don't bother doing any of this if we're not rendering the fragment
	{
		FragColor = texture(_Texture,fs_in.UV);
		FragColor = clamp(((FragColor) * pow(_ShellNumber + 1, _Attenuation) * (float(_ShellNumber)/float(_ShellsRendering))), FragColor*0.75, 1.5*FragColor);
	}
}