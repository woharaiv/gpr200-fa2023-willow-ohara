#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

out vec3 Normal;
uniform mat4 _Model;

void main(){
	Normal = vNormal;
	gl_Position = _Model * vec4(vPos,1.0);
	//OpenGL uses left-handed Y-up space (meaning Z goes away from the screen), but our transformations assumed a right-handed Y-up space (meaning Z goes towards the screen).
	//This line fixes the orientation to work how we assumed it did.
	gl_Position.z *= -1.0;
}