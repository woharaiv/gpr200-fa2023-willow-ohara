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

uniform float _ColorThresholdDecay;

uniform float _Attenuation;

void main(){
	float height = length(texture(_HairMap,fs_in.UV));
	if (height < (1 + _ColorThresholdDecay * _ShellNumber)) discard;
	else //Don't bother doing any of this if we're not rendering the fragment
	{
		FragColor = texture(_Texture,fs_in.UV);
		FragColor = clamp((FragColor * pow(_ShellNumber + 1, _Attenuation)), vec4(0, 0, 0, 0), 1.5*FragColor);
	}
}