#pragma once
#include "../ew/mesh.h"

namespace willowLib {
	enum ringType
	{
		TOP_FACE,
		BOTTOM_FACE,
		TOP_EDGE,
		BOTTOM_EDGE,
		ANGLED
	};
	void pushCircle(std::vector<ew::Vertex>* verticiesList, int subdivisions, float radius, ringType ringType, ew::Vec3 posOffset = { 0, 0, 0 }, float angOffset = 0);
	ew::MeshData createSphere(float radius, int numSegments);
	ew::MeshData createCylinder(float height, float radius, int numSegments);
	ew::MeshData createPlane(float size, int subdivisions);
	ew::MeshData createTorus(float thickness, float radius, int subdivisions);
}