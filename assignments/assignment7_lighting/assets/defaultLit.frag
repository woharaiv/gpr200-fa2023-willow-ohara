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

uniform float _diffuseK;
uniform float _specular;

uniform float _shininess;
vec3 reflectionVec;
uniform vec3 _cameraPos;
vec3 viewingAngle;
uniform bool _blinnPhong = false;

uniform sampler2D _Texture;

float diffuseFactor;
float specularFactor;

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 lightDir = normalize(_Light.position - fs_in.WorldPosition);
	FragColor = texture(_Texture,fs_in.UV);
	//Diffuse light
	diffuseFactor = max(dot(normal, lightDir), 0) * _diffuseK;
	//Reflection
	viewingAngle = normalize(_cameraPos - fs_in.WorldPosition);
	
	if (_blinnPhong)
	{
		vec3 h = normalize(lightDir + viewingAngle);
		specularFactor = pow(max(dot(h, normal), 0), _shininess);	
	}
	else
	{
		reflectionVec = 2 * dot(lightDir, normal) * normal - lightDir;
		specularFactor = pow(max(dot(reflectionVec, viewingAngle), 0), _shininess);
	}
	specularFactor *= _specular;
	
	vec3 diffuseColor = _Light.color * diffuseFactor;
	vec3 specularColor = _Light.color * specularFactor;
	//Combine lights
	FragColor = vec4(FragColor.rgb * (diffuseColor + specularColor), 1);
}