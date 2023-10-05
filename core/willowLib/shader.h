#pragma once
#include "../ew/external/glad.h"
#include "../ew/ewMath/mat4.h"
#include <string>

namespace willowLib {
	
	struct Vertex {
		float x, y, z;
		float u, v;
	};

	class Shader
	{
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use();
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setFloatArray(const std::string& name, float* v, int size) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setMat4(const std::string& name, const ew::Mat4& v) const;
	private:
		unsigned int m_id; //OpenGL program handle
	};

	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createShader(GLenum shaderType, const char* sourceCode);
	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
	unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
}