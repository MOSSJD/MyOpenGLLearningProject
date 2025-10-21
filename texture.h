#pragma once

#include <glad/glad.h>
#include <iostream>

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
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;
		GLuint& getGlID();
		~Texture();
	};
}