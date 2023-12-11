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
	vec2 positionInTexel = fract(vec2(fs_in.UV * hairMapSize));
	//How far is the current UV position from the center of the hair map texel is sits on?
	float distFromTexelCenter = distance(vec2(0.5, 0.5), positionInTexel);
	//Brighter texel means longer strand
	float height = texture(_HairMap,fs_in.UV).r;
	//Don't render the fragment if the strand is too tall or too wide
	if (distFromTexelCenter > _HairCutoffSlope*(height - (float(_ShellNumber)/_ShellsRendering)) && _ShellNumber > 0) discard;
	else //Don't bother doing any of this if we're not rendering the fragment
	{
		FragColor = texture(_Texture,fs_in.UV);
		FragColor *= pow(float(_ShellNumber + 1)/_ShellsRendering, _Attenuation);
	}
}