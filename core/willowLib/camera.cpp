#include "camera.h"
#include "transformations.h"

namespace willowLib {
	ew::Mat4 Camera::ViewMatrix() 
	{
		return willowLib::LookAt(position, target);
	}
	ew::Mat4 Camera::ProjectionMatrix()
	{
		if (orthogrpahic)
			return willowLib::Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
		else
			return willowLib::Perspective(fov * ew::DEG2RAD, aspectRatio, nearPlane, farPlane);
	}
}