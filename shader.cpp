#include "shader.h"

namespace ME {
	Shader::Shader(const char* vertexPath, const char* fragmentPath) {
		std::ifstream vertexFile;
		std::ifstream fragmentFile;
		std::string vertexCode;
		std::string fragmentCode;

		vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vertexFile.open(vertexPath);
			fragmentFile.open(fragmentPath);
			std::stringstream vertexStream, fragmentStream;
			vertexStream << vertexFile.rdbuf();
			fragmentStream << fragmentFile.rdbuf();
			vertexFile.close();
			fragmentFile.close();
			vertexCode = vertexStream.str();
			fragmentCode = fragmentStream.str();
		}
		catch (const std::ifstream::failure& e) {
			throw ShaderException(std::string("ERROR::SHADER::FAIL_TO_READ_SHADER_FILE\n") + e.what());
		}

		removeBOM(vertexCode);
		removeBOM(fragmentCode);
		const char* vertexCodeString = vertexCode.c_str();
		const char* fragmentCodeString = fragmentCode.c_str();

		unsigned int vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexCodeString, NULL);
		glCompileShader(vertexShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			glDeleteShader(vertexShader);
			throw ShaderException(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + infoLog);
		}

		unsigned int fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentCodeString, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			throw ShaderException(std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") + infoLog);
		}

		unsigned int shaderProgram;
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		glLinkProgram(shaderProgram);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			glDeleteProgram(shaderProgram);
			throw ShaderException(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + infoLog);
		}

		ID = shaderProgram;
	}

	Shader::~Shader() {
		glDeleteProgram(ID);
	}

	void Shader::use() const{
		glUseProgram(ID);
	}

	void Shader::setBool(const std::string &name, bool value) const{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
	}

	void Shader::setInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::setFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
	}

	void Shader::setMatrix4f(const std::string& name, const glm::f32mat4& value) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

}