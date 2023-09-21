#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec3 _ColorTopRise;
uniform vec3 _ColorBotRise;
uniform vec3 _ColorTopSet;
uniform vec3 _ColorBotSet;
uniform vec3 _BuildingColorRise;
uniform vec3 _BuildingColorSet;
uniform float _Speed;
uniform float _Time;
uniform vec2 _Resoultion;
uniform float _BuildingHeight[10];

float circleSDF(vec2 p, float r)
{
    return length(p)-r;
}

void main()
{
	//0 to 1
	vec2 uv = UV/_Resoultion.xy;
	//-1 to 1
	uv= uv * 2.0 - 1.0;
	//Correct for aspect ratio
	uv.x*=_Resoultion.x / _Resoultion.y;

	float tSin = (sin(_Time/(1/_Speed)) + 0.5) / 2;
	float tCos = (cos(_Time/(1/_Speed)) + 0.5) / 2;

	vec3 buildingBrightness = mix(_BuildingColorRise, _BuildingColorSet, tSin);
	vec3 topColor = mix(_ColorTopRise, _ColorTopSet, tSin);
	vec3 botColor = mix(_ColorBotRise, _ColorBotSet, tSin);
	vec3 color = mix(topColor,botColor,UV.y);
	float buildings = 1.0 - step(_BuildingHeight[int(floor(UV.x * 10))], UV.y);
	FragColor = vec4(mix(color,buildingBrightness,buildings), 1);
}