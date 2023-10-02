#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _ColorNoise;
uniform float _Time;

void main(){
	
	vec2 UVScrollDown = vec2(UV.x, UV.y + _Time * 0.6);
	vec2 UVScrollDiag = vec2(UV.x + _Time * 0.7, UV.y - _Time * 0.8);
	
	FragColor = texture(_Texture, UVScrollDown);

	//If the fragment isn't white, modify its color by the color perlin noise
	if (FragColor.r < 1.0 || FragColor.g < 1.0 || FragColor.b < 1.0)
	{
		FragColor += texture(_ColorNoise, UVScrollDiag);
		
		//If the color wouldn't be too dark when modulo'd by 1, do that
		if(FragColor.r > 1.5)
			FragColor.r = mod(FragColor.r, 1.0);
		if(FragColor.g > 1.5)
			FragColor.g = mod(FragColor.g, 1.0);
		if(FragColor.b > 1.5)
			FragColor.b = mod(FragColor.b, 1.0);
	}
}