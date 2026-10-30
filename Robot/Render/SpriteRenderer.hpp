#pragma once

#include <Robot/Common/Emotion.hpp>

#include <Robot/Render/SharedResourcesArray.hpp>

#include <Robot/Render/Image.hpp>

#include <GLFW/glfw3.h>

#include <algorithm>

#include <map>
#include <set>
#include <string>

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>


struct Sprite
{
	GLuint vao = 0;
	uint32_t vertexCount = 0;
	std::string shader;
	GLuint texture;

	glm::vec3 position;
	glm::vec3 scale;

	glm::vec3 color;

	glm::mat4 transform;

	void updateTransform();
};

class SpriteRenderer
{
public:
	SpriteRenderer(SharedResourceArray & resources);
	glm::vec2 windowSize();
	void setProjectionMatrix(glm::mat4 projection);
	void initializeDefaults();
	std::optional<Sprite> createSprite(std::string meshKey, std::string shaderKey, std::string textureKey);
	std::optional<Sprite> createSprite(std::string meshKey, std::string shaderKey);
	void push(Sprite instance);
	void draw();


private:
	struct Graph
	{
		std::vector<GLuint> vaos;
		std::vector<std::vector<ShaderProgram>> programs;
		std::vector<std::vector<std::vector<Sprite>>> instances;
	};

	Graph sortRenderList();

private:
	glm::mat4 projection_ {1.0f};
	std::vector<Sprite> renderList_;

private:
	SharedResourceArray & resourceArray_;
};
