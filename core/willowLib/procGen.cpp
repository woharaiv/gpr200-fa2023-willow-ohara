#include "procGen.h"

namespace willowLib {
	ew::MeshData createSphere(float radius, int numSegments)
	{
		ew::MeshData mesh;
		return mesh;
	}
	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		ew::MeshData mesh;
		return mesh;
	}
	ew::MeshData createPlane(float size, int subdivisions)
	{
		ew::MeshData mesh;
		int columns = subdivisions + 1;
		for (int row = 0; row <= columns; row++)
		{
			for (int col = 0; col <= columns; col++)
			{
				ew::Vertex v;
				v.pos.x = col * (size / columns);
				v.pos.y = 0;
				v.pos.z = row * (size / columns);
				v.normal = ew::Vec3(0, 1, 0);
				v.uv = { ((float)col / (columns - 1)), ((float)row / (columns - 1)) };
				mesh.vertices.push_back(v);
			}
		}
		for (int row = 0; row < subdivisions; row++)
		{
			for (int col = 0; col < subdivisions; col++)
			{
				int start = row * columns + col;
				//Bottom Right
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + columns + 1);
				//Top Left
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + columns);
			}
		}
		return mesh;
	}
}