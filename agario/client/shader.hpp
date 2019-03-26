#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// todo: remove this
#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

class Shader {
public:
	unsigned int ID;

	Shader() = default;

	// generates the shader on the fly
	Shader(const char* vertexPath, const char* fragmentPath) : ID(0) {
		generate_shader(vertexPath, fragmentPath);
	}

	void generate_shader(const char* vertexPath, const char* fragmentPath) {
		std::string vertexCode = get_file_contents(vertexPath);
		std::string fragmentCode = get_file_contents(fragmentPath);
		compile_shaders(vertexCode, fragmentCode);
	}

	std::string get_file_contents(const char* path) {
		std::string contents;
		std::ifstream file;

		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			file.open(path);
			std::stringstream stream;
			stream << file.rdbuf();
			file.close();
			return stream.str();
		} catch (std::ifstream::failure &e) {
			std::cerr << "SHDER FILE \"" << path << "\" not successfully read" << std::endl;
		}
		return std::string();
	}

	void compile_shaders(const std::string &vertexCode, const std::string &fragmentCode) {
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();

		unsigned int vertex, fragment;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use() {
		glUseProgram(ID);
	}

	void setBool(const std::string &name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec3(const std::string &name, float value1, float value2, float value3) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
	}

private:

	// utility function for checking shader compilation/linking errors.
	void checkCompileErrors(unsigned int shader, const std::string &type) {
		int success;
		char infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
									<< type << "\n" << infoLog << "\n" << std::endl;
			}
		} else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
									<< "\n" << infoLog << "\n" << std::endl;
			}
		}
	}
};
