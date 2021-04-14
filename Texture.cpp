#include "Texture.hpp"

Texture::Texture(const std::string& path, const int& force_channels)
{
	// Load image data
	unsigned char* data = SOIL_load_image(path.c_str(), &this->width, &this->height, 0, force_channels);

	// Check, if we have right path
	assert(data && "ERROR::TEXTURE::LOAD_FILE");

	// Generate gl texture
	glGenTextures(1, &this->id);

	// Setting texture context
	glBindTexture(GL_TEXTURE_2D, this->id);
	if (force_channels == SOIL_LOAD_RGBA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else if (force_channels == SOIL_LOAD_RGB)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// Automate generation mipmap texture
	glGenerateMipmap(GL_TEXTURE_2D);

	// Setting filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Free mem data
	SOIL_free_image_data(data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::getID() const
{
	return this->id;
}

int Texture::getWidth() const
{
	return this->width;
}

int Texture::getHeight() const
{
	return this->height;
}