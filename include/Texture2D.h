#pragma once

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

class Texture2D {
private:
	unsigned int ID = 0;
	int width = 0;
	int height = 0;
	int number = 0;

public:
	Texture2D(const char* texturePath, int number = 0) {
		this->number = number;

		glGenTextures(1, &this->ID);
		glBindTexture(GL_TEXTURE_2D, this->ID);
		int nrChannels;
		unsigned char* data = stbi_load(texturePath, &this->width, &this->height, &nrChannels, 0);
		if (data)
		{
			GLenum format;
			if (nrChannels == 3) format = GL_RGB;
			else if (nrChannels == 4) format = GL_RGBA;
			else if (nrChannels == 1) format = GL_RED;
			else {
				std::cout << "# channels: " << nrChannels << std::endl;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, format, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
	~Texture2D() {
		glDeleteTextures(1, &this->ID);
	}

	int getID() { return this->ID; }
	int getWidth() { return this->width; }
	int getHeight() { return this->height; }

	void bind() {
		glActiveTexture(GL_TEXTURE0 + this->number);
		glBindTexture(GL_TEXTURE_2D, this->ID);
	}
	void setIntParameter(GLenum parameter, GLint value) {
		glTexParameteri(GL_TEXTURE_2D, parameter, value);
	}
	void setFloatParameter(GLenum parameter, GLfloat value) {
		glTexParameterf(GL_TEXTURE_2D, parameter, value);
	}
};