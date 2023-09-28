#include "texture.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"
namespace willowLib {
	unsigned int loadTexture(const char* filePath, GLint wrapMode, GLint filterMode, GLint separateVerticalWrapMode = 0, GLint separateMipmapFilterMode = 0)
	{
		stbi_set_flip_vertically_on_load(true);

		int width, height, numComponents;
		unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
			if (data == NULL)
			{
				printf("Failed to load image %s", filePath);
				stbi_image_free(data);
				return 0;
			}
		
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		GLint format = GL_RGBA;
		switch (numComponents)
		{
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		default:
			break;
		}
		if (separateVerticalWrapMode == 0)
			separateVerticalWrapMode = wrapMode;
		if (separateMipmapFilterMode == 0)
		{
			if (filterMode == GL_NEAREST)
				separateMipmapFilterMode = GL_NEAREST_MIPMAP_NEAREST;
			else 
				separateMipmapFilterMode = GL_LINEAR_MIPMAP_LINEAR;
		}
		
		
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, separateVerticalWrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, separateMipmapFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		return texture;
	}
}