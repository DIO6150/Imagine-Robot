#pragma once

#include <string>
#include <vector>

#include <stdint.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Image
{
	std::vector<uint8_t> pixels;
	int32_t width;
	int32_t height;
	int32_t channels;

	void load(std::string path);
	void loadSinglePixelTexture(glm::vec3 color);
	GLuint createTexture();
};