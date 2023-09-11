#pragma once
#include "../ew/external/glad.h"

namespace willowLib {
	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createShader(GLenum shaderType, const char* sourceCode);
}