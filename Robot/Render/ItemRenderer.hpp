#pragma once

#include <Robot/Common/Emotion.hpp>

#include <Robot/Render/SharedResourcesArray.hpp>

#include <map>
#include <string>

#include <glad/glad.h>


struct ItemInstance
{
	GLuint emotionTexture = 0;
	glm::vec3 intensityColor {1.f, 1.f, 1.f};
	glm::mat4 transform {1.f};
};

class ItemRenderer
{
public:
	void init(SharedResources * resources);

	ItemInstance createItem(Emotion emotion, Intensity intensity);
	void render(ItemInstance item);

	void draw();

private:
	std::map<Emotion, GLuint> textures_;
	std::map<Intensity, glm::vec3> colors_;

	std::vector<ItemInstance> renderList_;

private:
	SharedResources * resources_;
};