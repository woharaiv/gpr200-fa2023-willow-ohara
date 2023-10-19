#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace willowLib {
	struct Camera {
		ew::Vec3 position = ew::Vec3(0, 0, 5); //Camera "eye" position
		ew::Vec3 target = ew::Vec3(0, 0, 0); //Position to look at
		ew::Vec3 f = ew::Vec3(0, 0, -1);
		float fov = 60; //Field of view in degrees
		float aspectRatio = 4/3; //Screen width / screen height
		float nearPlane = 0.1; //Near plane distance (+Z)
		float farPlane = 100; //Far plane distance (+Z)
		bool orthogrpahic = false; //Orhtographic or perspective?
		float orthoSize = 6; //Height of orthogrpahic frustum
		ew::Mat4 ViewMatrix(); //World -> view
		ew::Mat4 ProjectionMatrix(); //View -> clip

		const ew::Vec3 START_POS = position;
		const ew::Vec3 START_TARGET = target;
		const float START_FOV = fov;
		const float START_NEAR_PLANE = nearPlane;
		const float START_FAR_PLANE = farPlane;
		const float START_ORTHO_SIZE = orthoSize;
		void ResetCam()
		{
			position = START_POS;
			target = START_TARGET;
			fov = START_FOV;
			nearPlane = START_NEAR_PLANE;
			farPlane = START_FAR_PLANE;
			orthoSize = START_ORTHO_SIZE;
		}
	};
	struct CameraControls {
		double prevMouseX, prevMouseY; //Mouse position of previous frame
		float yaw = 0, pitch = 0; //In Degrees
		float mouseSensitivity = 10.0f;
		bool firstMouse = true; //Flag to help store initial mouse position
		float moveSpeed = 5.0f;
		bool invertY = false;
		
		const float START_YAW = yaw;
		const float START_PITCH = pitch;
		void ResetCamControls()
		{
			yaw = START_YAW;
			pitch = START_PITCH;
		}
	};
}