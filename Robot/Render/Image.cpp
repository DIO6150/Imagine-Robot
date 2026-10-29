
#include <Robot/Render/Image.hpp>

#include <stb/stb_image.h>

#include <iostream>

void Image::load(std::string path)
{
	auto * px = stbi_load(path.c_str(), &width, &height, &channels, 4);
	pixels.clear();
	
	if (!px)
	{
		width    = 0;
		height   = 0;
		channels = 0;
		std::cout << "Error: couldnt load texture : " << path << "\n";
		return;
	}
	
	auto pixelCount = width * height * channels;
	pixels.reserve(pixelCount);

	pixels.assign(px, px + pixelCount);
	stbi_image_free(px);
}

void Image::loadSinglePixelTexture(glm::vec3 color)
{
	pixels.clear();

	uint8_t r = 255 * color.r;
	uint8_t g = 255 * color.g;
	uint8_t b = 255 * color.b;

	pixels.push_back(r);
	pixels.push_back(g);
	pixels.push_back(b);
	pixels.push_back(255);

	width    = 1;
	height   = 1;
	channels = 4;
}

GLuint Image::createTexture()
{
	if (pixels.empty())
		return 0;

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}