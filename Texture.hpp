#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

// C++
#include <iostream>
#include <cassert>
#include <string>

// OpenGL
#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

class Texture
{
public:
	Texture(const std::string& path, const int& force_channels = SOIL_LOAD_RGB);

	unsigned int getID() const;
	int getWidth() const;
	int getHeight() const;

private:
	unsigned int id;
	int width, height;
};

#endif	// TEXTURE_H