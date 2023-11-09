#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};
#define MAX_LIGHTS 4
uniform Light _Light;

float intensity;

uniform float _diffuseK;

uniform float shininess;
vec3 reflectionVec;
uniform vec3 cameraPos;
vec3 viewingAngle;
uniform bool blinnPhong = false;

uniform sampler2D _Texture;

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 lightDir = normalize(_Light.position - fs_in.WorldPosition);
	FragColor = texture(_Texture,fs_in.UV);
	//Diffuse light
	intensity = max(dot(normal, lightDir), 0);
	//Reflection
	viewingAngle = normalize(cameraPos - fs_in.WorldPosition);
	reflectionVec = 2 * dot(lightDir, normal) * normal - lightDir;
	if (blinnPhong)
		intensity += pow(max(dot((lightDir + viewingAngle)/normalize(lightDir + viewingAngle), viewingAngle), 0), shininess);	
	else
		intensity *= pow(max(dot(reflectionVec, viewingAngle), 0), shininess);
	//Combine lights
	FragColor = FragColor * _diffuseK * intensity;
}