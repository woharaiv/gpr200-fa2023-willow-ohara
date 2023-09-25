#version 450
out vec4 FragColor;
in vec2 UV;
uniform vec3 _ColorTopRise, _ColorBotRise, _ColorTopSet, _ColorBotSet;
uniform vec4 _BuildingColorRise, _BuildingColorSet;
uniform vec4 _SunColorRise, _SunColorSet;
uniform float _SunRadius, _SunOrbitRadius;
uniform vec2 _SunOrbitPos;
uniform float _Speed;
uniform float _Time;
uniform vec2 _Resolution;
uniform float _BuildingHeight[10];

float circleSDF(vec2 p, float r)
{
    return length(p)-r;
}

void main()
{
	//Correct UV for resolution (for making round circles)
	vec2 correctedUV = UV * 2.0 - 1.0;
	correctedUV.x*=_Resolution.x / _Resolution.y;

	//Sine and Cosine waves ocelating between 0 and 1 across time
	float tSin = (sin(_Time/(1/_Speed)) + 0.5) / 2;
	float tCos = (cos(_Time/(1/_Speed)) + 0.5) / 2;

	//Sky color
	vec3 topColor = mix(_ColorTopRise, _ColorTopSet, tCos);
	vec3 botColor = mix(_ColorBotRise, _ColorBotSet, tCos);
	vec3 skyColor = mix(botColor,topColor,UV.y);
	
	//Sun color
	vec4 sunRiseColor = _SunColorRise;
	vec4 sunSetColor = _SunColorSet;
	vec4 sunColor = mix(sunRiseColor, sunSetColor, tCos);
	
	//Sun position
	vec2 sunPos = vec2((tSin - _SunOrbitPos.x) * -_SunOrbitRadius, (tCos - _SunOrbitPos.y) * -_SunOrbitRadius);
	float sun = 1.0 - circleSDF(correctedUV - sunPos, _SunRadius - 1);
	sun = smoothstep(-0.05,0.5,sun);
	vec3 sunSky = mix(skyColor, sunColor.rgb, sun * sunColor.a);

	//Buildings
	float buildings = 1.0 - step(_BuildingHeight[int(floor(UV.x * 10))], UV.y);
	vec4 buildingBrightness = mix(_BuildingColorRise, _BuildingColorSet, tCos);
	FragColor = vec4(mix(sunSky,buildingBrightness.rgb,buildings * buildingBrightness.a), 1);

}