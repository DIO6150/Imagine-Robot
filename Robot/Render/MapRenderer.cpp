
#include <Robot/Render/MapRenderer.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

#include <iostream>

#include <Robot/Render/Image.hpp>

void MapRenderer::init(SharedResources * resources)
{
	resources_ = resources;

	stbi_set_flip_vertically_on_load(true);

	Image blank, wall, dictionnary, empty, custommer, chest;
	
	custommer  .load("data/assets/textures/guy.png");
	dictionnary.load("data/assets/textures/dictionnary.png");
	empty      .load("data/assets/textures/empty.png");
	wall       .load("data/assets/textures/wall.png");
	chest      .load("data/assets/textures/chest.png");
	blank      .loadSinglePixelTexture({1.f, 1.f, 1.f});

	textures_[Custommer]  = custommer  .createTexture();
	textures_[Dictionary] = dictionnary.createTexture();
	textures_[Empty]      = empty      .createTexture();
	textures_[Wall]       = wall       .createTexture();
	textures_[Blank]      = blank      .createTexture();
	textures_[Chest]      = chest      .createTexture();

	textures_[Agent] = textures_[Custommer];

	agent_.albedoTexture = textures_[Agent];
	agent_.albedoColor   = {1.0, 0.0, 0.0};
}

void MapRenderer::setMap(std::vector<Tile> const & map, uint32_t width, uint32_t height)
{
	mapWidth_  = width;
	mapHeight_ = height;

	tileList_.clear();
	tileList_.reserve(map.size());

	const float targetSize = 1.5f;
	scale_ = targetSize / std::max(width, height);

	mapTransform_ = glm::mat4{1.0f};

	mapTransform_ = glm::translate(
		mapTransform_,
		glm::vec3
		{
			-scale_ * width  * .5f,
			-scale_ * height * .5f,
			.0f
		}
	);

	mapTransform_ = glm::scale(
		mapTransform_,
		{scale_, scale_, 1.0f}
	);
	
	for (auto tile : map)
	{
		Sprite obj;
		
		glm::vec3 position {tile.pos_.c, (height - 1) - tile.pos_.l, .0f};
		obj.transform = glm::translate(mapTransform_, position);

		obj.albedoTexture = textures_[Blank];

		if (tile.isEmpty())
			obj.albedoTexture = textures_[Empty];
		else if (tile.isPerson())
		{
			obj.albedoColor   = {0.f, 0.f, 1.f};
			obj.albedoTexture = textures_[Custommer];
		}
		else if (tile.isRecord())
			obj.albedoTexture = textures_[Dictionary];
		else if (tile.isAgentStart())
		{
			obj.albedoTexture = textures_[Empty];
			agent_.transform = obj.transform;
		}
		else if (tile.isItemPickup())
			obj.albedoTexture = textures_[Chest];
		else if (tile.isSolid())
			obj.albedoTexture = textures_[Wall];

		if (tile.isPerson())
		{
			Sprite floor;
			floor.transform = obj.transform;
			floor.albedoTexture = textures_[Empty];
			tileList_.push_back(floor);
		}
		tileList_.push_back(obj);
	}
}

void MapRenderer::setAgentPosition(glm::vec2 pos)
{
	agent_.transform = glm::translate(glm::mat4{1.f}, glm::vec3{pos, .0f});
	agent_.transform = glm::scale(agent_.transform, glm::vec3 { scale_, scale_, 1.f});
}

void MapRenderer::setAgentColor(glm::vec3 color)
{
	agent_.albedoColor = color;
}

void MapRenderer::draw()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(resources_->rectVao);
	resources_->rectProgram.use();
	for (auto tile : tileList_)
	{
		resources_->rectProgram.uploadMatrix4("u_transform", tile.transform);

		if (tile.albedoTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tile.albedoTexture);
		}

		resources_->rectProgram.upload3fv("u_color", tile.albedoColor);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	resources_->rectProgram.uploadMatrix4("u_transform", agent_.transform);
	if (agent_.albedoTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, agent_.albedoTexture);
	}
	resources_->rectProgram.upload3fv("u_color", agent_.albedoColor);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
}

glm::vec2 MapRenderer::from(Pos2D tilePos) const
{
	glm::vec3 position {tilePos.c, (mapHeight_ - 1) - tilePos.l, .0f};
	auto m = glm::translate(mapTransform_, position);

	return { m[3][0], m[3][1] };
}