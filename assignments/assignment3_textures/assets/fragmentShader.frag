#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _WallTexture;
uniform sampler2D _GuyTexture;

void main(){
	//FragColor = vec4(UV.x,UV.y,0.0,1.0);
	
	FragColor = mix(texture(_WallTexture, UV), texture(_GuyTexture, UV*2), 1);
}