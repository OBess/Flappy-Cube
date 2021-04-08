#pragma once

#ifndef SHADER_HPP
#define SHADER_HPP

// C++
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// OpenGL
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
	explicit Shader(const std::string& vertexShader, const std::string& fragmentShader);
	explicit Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	GLuint getID() const;

	void Use();

private:
	GLuint _id;
};

#endif // SHADER_HPP
