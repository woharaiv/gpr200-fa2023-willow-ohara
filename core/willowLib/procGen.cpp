#include "procGen.h"

namespace willowLib {
	//Helper function to push a ring of vertexes to mesh.verticies, specifically for cylinders.
	void pushCylinderCircle(std::vector<ew::Vertex>* verticiesList, int subdivisions, float radius, ringType normalsDir, ew::Vec3 posOffset, float angOffset)
	{
		float thetaStep = ew::TAU / subdivisions;
		for (int i = 0; i <= subdivisions; i++)
		{
			ew::Vertex v;
			float theta = i * thetaStep;
			v.pos.x = (cos(theta) * radius) + posOffset.x;
			v.pos.y = posOffset.y;
			v.pos.z = (sin(theta) * radius) + posOffset.z;
			switch (normalsDir)
			{
				case TOP_FACE:
					v.normal = { 0, 1, 0 };
					v.uv = { (cos(theta) + 1) / 2, (sin(theta) + 1) / 2 };
					break;
				case BOTTOM_FACE:
					v.normal = { 0, -1, 0 };
					v.uv = { (cos(theta) + 1) / 2, (sin(theta) + 1) / 2 };
					break;
				case TOP_EDGE:
					v.normal = { cos(theta), 0, sin(theta) };
					v.uv = { theta / ew::TAU, 1 };
					break;
				case BOTTOM_EDGE:
					v.normal = { cos(theta), 0, sin(theta) };
					v.uv = { theta / ew::TAU, 0 };
					break;
			}
			verticiesList->push_back(v);
		}
	}
	ew::MeshData createSphere(float radius, int numSegments)
	{
		ew::MeshData mesh;
		return mesh;
	}
	//Create a cylinder, centered on the middle of the shape.
	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		ew::MeshData mesh;
		float topY = height / 2;
		float bottomY = -topY;
		//Top center point
		ew::Vertex topPoint;
		topPoint.pos = { 0, topY, 0 };
		topPoint.normal = { 0, 1, 0 };
		topPoint.uv = { 0.5, 0.5 };
		mesh.vertices.push_back(topPoint);
		//Top face-normal ring
		pushCylinderCircle(&mesh.vertices, numSegments, radius, TOP_FACE, { 0, topY, 0 });
		//Top side-normal ring
		int sideIndiciesStart = mesh.vertices.size();
		pushCylinderCircle(&mesh.vertices, numSegments, radius, TOP_EDGE, { 0, topY, 0 });
		//Bottom side-normal ring
		pushCylinderCircle(&mesh.vertices, numSegments, radius, BOTTOM_EDGE, { 0, bottomY, 0 });
		//Bottom face-normal ring
		int bottomIndiciesStart = mesh.vertices.size();
		pushCylinderCircle(&mesh.vertices, numSegments, radius, BOTTOM_FACE, { 0, bottomY, 0 });
		//Bottom center point
		ew::Vertex bottomPoint;
		bottomPoint.pos = { 0, bottomY, 0 };
		bottomPoint.normal = { 0, -1, 0 };
		bottomPoint.uv = { 0.5, 0.5 };
		mesh.vertices.push_back(bottomPoint);

		//Connect top circle indices
		int startIndex = 1;
		int centerIndex = 0;
		for (int i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(startIndex + i);
			mesh.indices.push_back(centerIndex);
			mesh.indices.push_back(startIndex + i + 1);
		}
		//Connect bottom circle indices
		centerIndex = mesh.vertices.size() - 1;
		for (int i = 0; i <= numSegments; i++)
		{
			mesh.indices.push_back(bottomIndiciesStart + i);
			mesh.indices.push_back(centerIndex);
			mesh.indices.push_back(bottomIndiciesStart + i + 1);
		}

		//Connect side indices
		int columns = numSegments + 1;
		for (int i = 0; i < columns; i++)
		{
			int start = sideIndiciesStart + i;
			//Downward-pointing triangle
			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns);
			//Upward-pointing triangle
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + columns + 1);
			mesh.indices.push_back(start + columns);
		}
		return mesh;
	}
	//Create a plane, centered on the top left corner (when looking down)
	ew::MeshData createPlane(float size, int subdivisions)
	{
		ew::MeshData mesh;
		int columns = subdivisions + 1;
		for (int row = 0; row <= subdivisions; row++)
		{
			for (int col = 0; col <= subdivisions; col++)
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
				//Top Left
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + columns + 1);
				mesh.indices.push_back(start + columns);
				//Bottom Right
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + columns + 1);
				
			}
		}
		return mesh;
	}
}