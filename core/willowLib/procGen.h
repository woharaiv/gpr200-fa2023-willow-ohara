#pragma once
#include "../ew/mesh.h"
#include "../ew/transform.h"
#include "../ew/procGen.h"

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
	
	//A cube centered on the cube's center.
	struct Cube {
		float size = 1;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(ew::createCube(size)); }
	};
	//A sphere centered on the sphere's center
	struct Sphere {
		float radius = 1;
		int subdivisions = 8;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(createSphere(radius, subdivisions)); }
	};
	//A cylinder centered on the cylinder's center
	struct Cylinder {
		float height = 1, radius = 0.5f;
		int subdivisions = 8;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(createCylinder(height, radius, subdivisions)); }
	};
	//A plane centered on the plane's top-left corner (when looking top-down)
	struct Plane {
		float size = 1.5;
		int subdivisions = 8;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(createPlane(size, subdivisions)); }
	};
	//A torus, centered on the torus's center and laying across the XZ plane
	struct Torus {
		float radius = 0.5f, thickness = 0.25f;
		int subdivisions = 16;
		ew::Transform transform;
		ew::Mesh getMesh() { return ew::Mesh(createTorus(radius, thickness, subdivisions)); }
	};
}