#pragma once
namespace willowLib {
	unsigned int loadTexture(const char* filePath, GLint wrapMode, GLint filterMode, GLint separateVerticalWrapMode = 0, GLint separateMipmapFilterMode = 0);
}