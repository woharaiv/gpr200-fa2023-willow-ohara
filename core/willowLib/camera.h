#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace willowLib {
	struct Camera {
		ew::Vec3 position = (0, 0, 5); //Camera "eye" position
		ew::Vec3 target = (0, 0, 0); //Position to look at
		float fov = 60; //Field of view in degrees
		float aspectRatio = 4/3; //Screen width / screen height
		float nearPlane = 0.1; //Near plane distance (+Z)
		float farPlane = 100; //Far plane distance (+Z)
		bool orthogrpahic = true; //Orhtographic or perspective?
		float orthoSize = 6; //Height of orthogrpahic frustum
		ew::Mat4 ViewMatrix(); //World -> view
		ew::Mat4 ProjectionMatrix(); //View -> clip
	};
	struct CameraControls {
		double prevMouseX, prevMouseY; //Mouse position of previous frame
		float yaw = 0, pitch = 0; //In Degrees
		float mouseSensitivity = 0.1f;
		bool firstMouse = true; //Flag to help store initial mouse position
		float moveSpeed = 5.0f;
	};
}