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
	GLenum format;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;
	else
		throw ME::MyError("Unsupported texture format");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
ME::Texture::Texture(Texture&& other) noexcept {
	glID = other.glID;
	data = other.data;
	allocated = other.allocated;
	other.data = nullptr;
	other.allocated = false;
}
ME::Texture& ME::Texture::operator=(Texture&& other) noexcept {
	if (this != &other) {
		stbi_image_free(data);
		glDeleteTextures(1, &glID);
		glID = other.glID;
		data = other.data;
		allocated = other.allocated;
		other.data = nullptr;
		other.allocated = false;
	}
	return *this;
}
GLuint& ME::Texture::getGlID() {
	if (!allocated) {
		throw ME::MyError("Using uninitialized texture");
	}
	return glID;
}
ME::Texture::~Texture() {
	//std::cout << "~Texture freeing " << static_cast<void*>(data) << std::endl;
	stbi_image_free(data);
}