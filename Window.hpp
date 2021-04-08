#pragma once

#ifndef WINDOW_H
#define WINDOW_H

// C++
#include <iostream>

// OpenGL
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	explicit Window(const GLchar* title, const int& width, const int& height);
	~Window();

	const GLuint& getWidth() const;
	const GLuint& getWHeight() const;
	GLFWwindow* getWindow() const;

	void setKeyCallBack(GLFWkeyfun callback);

private:
	GLFWwindow* window;
	int width, height;
};

#endif // WINDOWS_H