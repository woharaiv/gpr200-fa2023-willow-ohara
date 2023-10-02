#pragma once
#include "../ew/external/glad.h"
namespace willowLib {
	unsigned int loadTexture(const char* filePath, GLint wrapMode, GLint filterMode, GLint separateVerticalWrapMode = 0, GLint separateMipmapFilterMode = 0);
}