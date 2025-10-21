#pragma once

#include <glad/glad.h>

#include "stb_image.h"
#include "util.h"

namespace ME {
	class Texture {
	private:
		bool allocated;
		GLuint glID;
		unsigned char* data;
	public:
		Texture();
		Texture(const char* path);
		GLuint getGlID() const;
		~Texture();
	};
}