#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// todo: remove this
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

class Shader {
public:
	GLuint program;

	Shader() { print_version(); };

	// generates the shader on the fly
	Shader(const char* vertex_path, const char* fragment_path) : program(0) {
    print_version();
		generate_shader(vertex_path, fragment_path);
	}

	void generate_shader(const char* vertex_path, const char* fragment_path) {
		std::string vertex_code = get_file_contents(vertex_path);
		std::string fragment_code = get_file_contents(fragment_path);
		compile_shaders(vertex_code, fragment_code);
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
			std::cerr << "SHADER FILE \"" << path << "\" not successfully read" << std::endl;
		}
		return std::string();
	}

	void compile_shaders(const std::string &vertex_code, const std::string &fragment_code) {
		const char* vShaderCode = vertex_code.c_str();
		const char* fShaderCode = fragment_code.c_str();

		unsigned int vertex, fragment;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

		// shader Program
		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
    check_compile_errors(program, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use() {
		glUseProgram(program);
	}

	void setBool(const std::string &name, bool value) const {
		glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);
	}

	void setVec3(const std::string &name, float value1, float value2, float value3) const {
		glUniform3f(glGetUniformLocation(program, name.c_str()), value1, value2, value3);
	}

	void print_version() {
    std::cout << "Shader version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  }

	~Shader() { glDeleteProgram(program); }

private:

	// utility function for checking shader compilation/linking errors.
	void check_compile_errors(unsigned int shader, const std::string &type) {
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
