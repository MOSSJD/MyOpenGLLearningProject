#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "Util.h"

namespace ME {
	class Shader {
	public:
		unsigned int ID;

		Shader(const char* vertexPath, const char* fragmentPath);
		~Shader();

		void use() const;
		void setBool(const std::string& name, bool value) const;
		void setFloat(const std::string& name, float value) const;
		void setInt(const std::string& name, int value) const;
		void setMatrix4f(const std::string& name, const glm::f32mat4& value) const;
	};

	class ShaderException : public ME::MyError {
	public:
		ShaderException(const std::string& message) : MyError(message) {}
		virtual ~ShaderException() {}
	};
};