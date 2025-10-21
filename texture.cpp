#include "texture.h"

ME::Texture::Texture() {
	allocated = false;
	glID = 0;
	data = nullptr;
}
ME::Texture::Texture(const char* path) {
	allocated = true;
	glGenTextures(1, &glID);
	glBindTexture(GL_TEXTURE_2D, glID);
	// Set Texture wraping configurations
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Loading a texture image
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data == nullptr) {
		throw ME::MyError("Fail to load texture image");
	}
	// Loading textures and generating mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
GLuint ME::Texture::getGlID() const {
	if (!allocated) {
		throw ME::MyError("Using uninitialized texture");
	}
	return glID;
}
ME::Texture::~Texture() {
	stbi_image_free(data);
}